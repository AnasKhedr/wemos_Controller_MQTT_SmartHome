#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include "relayDevice.hpp"
#include "mqttClient.hpp"
#include "helper.hpp"
#include <arduino-timer.h>
#include "relayDevice.hpp"
#include "Application.hpp"

#define LED D2
// Uncomment the type of sensor in use:

// unsigned long previousMillis = 0;
// unsigned long interval = 10000;

// const char *kitchenServer = "192.168.1.80";
// const char *bedRoomServer = "192.168.1.21";
// const int mqttPort = 1883;
// int t;
// int h;

app::Application myApp;

// WiFiClient espClient;
// PubSubClient client(kitchenServer, mqttPort, helper::mqttCallback, espClient);
// PubSubClient client_(bedRoomServer, mqttPort, callback_, espClient);

// mqtt::mqttClient kitchenClient(kitchenServer, "#");
// mqtt::mqttClient bedRoomClient(bedRoomServer, "#");
// Timer<2> timer; // = timer_create_default(); // create a timer with default settings

                            //5 means GPIO5 -> D1
// bathRoom::bathRoomGPIO Light1(5,bathRoom::GPIOtype::activeLow, "Light5");
// bathRoom::bathRoomGPIO Light2(D6,bathRoom::GPIOtype::activeLow, "Light6");
// bathRoom::bathRoomGPIO Light3(D7,bathRoom::GPIOtype::activeLow, "Light7");

void setup()
{
    // pinMode(10,OUTPUT);
    // put your setup code here, to run once:
    // Serial.begin(115200);
    Serial.println("start of setup");
    delay(200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    // WiFiManager wifiManager;
    //reset saved settings
    // wifiManager.resetSettings();

    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    // wifiManager.autoConnect("NodeMCU V3", "12345678__");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    //if you get here you have connected to the WiFi
    // Serial.println("connected to WIFI network.");

    Serial.println(__LINE__);
    myApp.addClient(TESTBROKER);
    // myApp.addClient(PI_4_1);
    // myApp.addClient(PI_4_2_kitchen);
    Serial.println(__LINE__);
    myApp.init();
    Serial.println(__LINE__);
    // kitchenClient.init();
    // timer.every(100, [](void*) -> bool
    // {
    //     kitchenClient.loop();
    //     return true;
    // });

    // timer.every(1000,[](void*) -> bool
    // {
    //     Serial.println("lambda expression:");
    //     t++;
    //     h++;
    //     Serial.println("Temp : " + String(t));
    //     Serial.println("Humi : " + String(h));
    //     String toSend = String(t) + "," + String(h);
    //     kitchenClient.publish("data", toSend.c_str());

    //     return true;
    // });

    // timer.every(1000, [](void*) -> bool
    // {
    //     Light1.toggel();
    //     return true;
    // });
    // Serial.println("end of setup.");
}

void loop()
{
    myApp.run();
    // // Serial.println("start of loop");
    // timer.tick();
    // // Light1.toggel();
    // // delay(2000);
    // kitchenClient.loop();
    // // Serial.println("after .loop");

    // // unsigned long currentMillis = millis();
    // // if (currentMillis - previousMillis >= interval)
    // // {
    // //     // save the last time you updated the DHT values
    // //     previousMillis = currentMillis;
    // //     if (1)
    // //     {
    // //         delay(2000);
    // //         Serial.println("main loop:");
    // //         t++;
    // //         h++;
    // //         Serial.println("Temperature : " + String(t));
    // //         Serial.println("Humidity : " + String(h));
    // //         String toSend = String(t) + "," + String(h);
    // //         kitchenClient.publish("data", toSend.c_str());
    // //     }
    // // }
}
