
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino   --> builtin V1.0.0

// needed for library
#include "Application.hpp"
#include "helper.hpp"
#include "mqttClient.hpp"
#include "relayDevice.hpp"
// #include <AsyncEventSource.h>
#include <DNSServer.h> // --> builtin V1.1.1
#include <ESP8266WebServer.h> // --> buitlin V1.0.0
#include <PubSubClient.h> //https://pubsubclient.knolleary.net/   --> V2.8.0
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager  --> V1.7.2
#include <arduino-timer.h> //https://github.com/contrem/arduino-timer  --> V2.3.0

#undef MAX_TIME_INACTIVE

app::Application myApp;

void setup()
{
    // Serial.begin(115200);

    myApp.init();
    // myApp.addBroker(TESTBROKER);
    // myApp.addBroker(PI_4_2_kitchen);
    // myApp.addBroker("192.168.1.8");
    // myApp.addBroker("nymea-bedroom");
    // myApp.addBroker("192.168.0.100");
    // myApp.addBroker(PI_4_2_kitchen);
    myApp.addBroker("nymea-kitchen");
    // myApp.addBroker("Anas-PC");
    myApp.setupBrokers();

    // Debug.begin("RemoteDebug");
    // Debug.setResetCmdEnabled(true); // Enable the reset command
    // Debug.showProfiler(true); // To show profiler - time between messages of Debug
    // Serial.begin(115200); // we're using a serial pin so this messes up the serial so we reinit the serial port
}

void loop()
{
    myApp.run();
    // Serial.printf("done creating GPIOs.\n");
    // delay(1000);
    Debug.handle();
    yield();
}
