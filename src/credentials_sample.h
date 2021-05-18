// Rename to "credentials.h" to use with main.cpp
#define WIFI_PASSWORD "*****"
#define WIFI_SSID "*****"
#define WIFI_HOSTNAME "ESP-MQTT-Client" //ESP hostname
#define MQTT_BROKER_HOST "orzhova"      //IP/Hostname of the MQTT broker
#define MQTT_BROKER_PORT 1883           //Port number of the broker
#define MQTT_BASE_TOPIC "test/"         //The base topic you MQTT publishes should start with, e.g. "room/device/"  bastian_office/
#define MQTT_PUBLISH_INTERVAL 20        //Publish interval in seconds
#define MQTT_QOS 0                      //MQTT QoS
#define MQTT_RETAIN false               //MQTT Retain Flag