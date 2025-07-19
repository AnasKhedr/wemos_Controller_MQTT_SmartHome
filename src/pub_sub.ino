#include "Application.hpp"
#include "common/helper.hpp"
#include "network/mqttClient.hpp"
#include "preferals/relay_device/relayDevice.hpp"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>

#define LED D2

app::Application myApp;

void setup()
{
    Serial.println("start of setup");

    myApp.addClient(TESTBROKER);
    myApp.init();
}

void loop()
{
    myApp.run();
}
