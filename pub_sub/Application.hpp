//---------------------------------------------------------------------------
//! \file Application.hpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 29-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 



//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
#include "relayDevice.hpp"
#include "mqttClient.hpp"
#include "helper.hpp"



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

class Application
{
    public:
        Application();
        // Data members

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        void init();

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        void run();

        void addClient(std::string brokerIP);

    private:
        // Data members
        // std::vector<std::pair<std::string, std::string>> m_brokerInitData;
        std::vector<std::string> m_brokerIps;

        // std::vector<mqtt::mqttClient> m_mqttClients;
        std::vector<std::unique_ptr<mqtt::mqttClient>> m_mqttClients;

        WiFiClient m_espClient;

        WiFiManager m_wifiManager;

        Timer<10> m_timerTasks;

        std::vector<bathRoom::bathRoomGPIO> m_ControlGPIOsList;
};

