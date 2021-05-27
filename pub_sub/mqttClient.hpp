//---------------------------------------------------------------------------
//! \file IMqttClient.hpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

#ifndef _MQTTClient_HPP
#define _MQTTClient_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Arduino.h>
#include <string>
#include <PubSubClient.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include "helper.hpp"



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
    typedef std::function<void(char*, uint8_t*, unsigned int)> callbackType;

    class mqttClient
    {
        // using callback = std::function<void(char*, uint8_t*, unsigned int)>;
        public:
            mqttClient(std::string ip, std::string topic, callbackType = helper::mqttCallback);
            // mqttClient(const char* ip, const char* topic, callback = helper::mqttCallback);
            // Data members

            void publish(const std::string& Topic, const std::string& payload);

            void loop();
            void init();

            PubSubClient& getClient();

        private:
            WiFiClient m_wifiClient;
            PubSubClient m_pubSubClient;
            // Data members

    };

}       // namespace mqtt

#endif      //_MQTTClient_HPP
