//---------------------------------------------------------------------------
//! \file mqttClient.hpp
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
#include "ImqttObserver.hpp"
#include "common.hpp"



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
    typedef std::function<void(char*, uint8_t*, unsigned int)> callbackType;

    //---------------------------------------------------------------------------
    //! \brief default callback, has no use except for debugging
    //! this function will be used in case of no callback was provided
    //! \param topic
    //! \param payload
    //! \param length
    //!
    void mqttCallback(char *topic, uint8_t *payload, unsigned int length);

    class mqttClient
    {
        // using callback = std::function<void(char*, uint8_t*, unsigned int)>;
        public:
            mqttClient(std::string ip, std::string subscriptionTopic, callbackType callback = mqttCallback);
            // mqttClient(std::string ip, std::string topic, callbackType callback = mqttCallback);
            // mqttClient(const char* ip, const char* topic, callback = helper::mqttCallback);
            // Data members

            void publish(const std::string& Topic, const std::string& payload);

            template<class T>
            void publish(const std::string& Topic, const T& payload);

            void loop();
            bool init();
            void publishState(const std::string& topic, const std::string& payload);
            void publishState(const std::string& topic, const helper::actions& action);

            PubSubClient& getClient();

        private:

            // Data members
            WiFiClient m_wifiClient;
            PubSubClient m_pubSubClient;
            std::string m_brokerIp;
            std::string m_subscriptionTopic;
            std::string m_clientId;
            unsigned long m_lastReconnectAttempt;

            //---------------------------------------------------------------------------
            //! \brief a variable to be set only if the broker was initalized successfully.
            //!
            bool isBrokerInitialized;

            std::vector<IObserver> m_observersList;


    };

}       // namespace mqtt

#endif      //_MQTTClient_HPP
