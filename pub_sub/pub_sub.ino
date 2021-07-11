
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino   --> builtin V1.0.0

//needed for library
#include <DNSServer.h>          // --> builtin V1.1.1
#include <ESP8266WebServer.h>   // --> buitlin V1.0.0
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager  --> V1.7.2
#include <PubSubClient.h>       //https://pubsubclient.knolleary.net/   --> V2.8.0
#include "relayDevice.hpp"
#include "mqttClient.hpp"
#include "helper.hpp"
#include <arduino-timer.h>      //https://github.com/contrem/arduino-timer  --> V2.3.0
#include "relayDevice.hpp"
#include "Application.hpp"

app::Application myApp;

void setup()
{
    // myApp.addClient(TESTBROKER);
    myApp.addClient(PI_4_1);
    // myApp.addClient("192.168.0.100");
    // myApp.addClient(PI_4_2_kitchen);
    myApp.init();

    Debug.begin("RemoteDebug");
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showProfiler(true); // To show profiler - time between messages of Debug
    debugA("This is a any (always showed) - var x\n");
}

void loop()
{
    myApp.run();
    Debug.handle();
    yield();
}
