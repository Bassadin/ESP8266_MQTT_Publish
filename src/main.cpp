#include <Arduino.h>

#include <credentials.h>

#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

Ticker mqttPublishTicker;

#define DHTPIN 12

#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

String humidity, temperature;

void readSensorAndPublishData()
{
    Serial.println("readSensorAndPublishData() call");

    mqttClient.publish((String(MQTT_BASE_TOPIC) + "temperature").c_str(), MQTT_QOS, MQTT_RETAIN, temperature.c_str());
    mqttClient.publish((String(MQTT_BASE_TOPIC) + "humidity").c_str(), MQTT_QOS, MQTT_RETAIN, humidity.c_str());
}

void connectToWifi()
{
    Serial.println("Connecting to Wi-Fi...");

    WiFi.mode(WIFI_STA);
    WiFi.hostname(WIFI_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT.");
    readSensorAndPublishData();
    mqttPublishTicker.attach(MQTT_PUBLISH_INTERVAL, readSensorAndPublishData);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.println("Disconnected from MQTT.");

    if (reason == AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT)
    {
        Serial.println("Bad server fingerprint.");
    }

    if (WiFi.isConnected())
    {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
    Serial.print("Connected to Wi-Fi: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    mqttPublishTicker.detach();
    wifiReconnectTimer.once(2, connectToWifi);
}

void setup()
{
    Serial.begin(9600);

    dht.begin();

    Serial.println();
    Serial.println();

    Serial.println(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    // Set delay between sensor readings based on sensor details.
    delayMS = sensor.min_delay / 1000;

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);

    mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);

    connectToWifi();
}

void loop()
{
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println(F("Error reading temperature!"));
    }
    else
    {
        temperature = event.temperature;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println(F("Error reading humidity!"));
    }
    else
    {
        humidity = event.relative_humidity;
    }
    // Delay between measurements.
    delay(delayMS);
}
