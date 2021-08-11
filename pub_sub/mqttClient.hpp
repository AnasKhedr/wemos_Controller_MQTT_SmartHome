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
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include "helper.hpp"
#include "ImqttObserver.hpp"
#include "common.hpp"

#include <Ticker.h>
#include <AsyncMqttClient.h>



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
    typedef std::function<void(char *topic, char *payload, size_t length)> callbackType;

    //---------------------------------------------------------------------------
    //! \brief default callback, has no use except for debugging
    //! this function will be used in case of no callback was provided
    //! \param topic
    //! \param payload
    //! \param length
    //!
    void mqttCallback(char *topic, char *payload, size_t length);

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

            //---------------------------------------------------------------------------
            //! \brief 
            //! 
            //! \return true
            //! \return false
            //!
            bool loop();

            //---------------------------------------------------------------------------
            //! \brief 
            //! 
            //! \return true
            //! \return false
            //!
            bool init();

            //---------------------------------------------------------------------------
            //! \brief 
            //! 
            //! \param topic
            //! \param payload
            //!
            void publishState(const std::string& topic, const std::string& payload);

            //---------------------------------------------------------------------------
            //! \brief 
            //! 
            //! \param topic
            //! \param action
            //!
            void publishState(const std::string& topic, const bool& state);

            bool reconnect();

            AsyncMqttClient& getClient();

        private:

            // Data members
            WiFiClient m_wifiClient;

            AsyncMqttClient m_mqttClient;

            std::string m_brokerIp;
            std::string m_subscriptionTopic;
            unsigned long m_lastReconnectAttempt;

            bool m_isConnected;

            callbackType m_callback;

            //---------------------------------------------------------------------------
            //! \brief a variable to be set only if the broker was initalized successfully.
            //!
            bool isBrokerInitialized;

            std::vector<IObserver> m_observersList;

            void onMqttConnect(bool sessionPresent);

            void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);

            void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);


    };

}       // namespace mqtt

#endif      //_MQTTClient_HPP
