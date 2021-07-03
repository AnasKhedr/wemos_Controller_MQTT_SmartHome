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
#include <Adafruit_ADS1015.h>   // for m_ads version 1.1.1
#include <arduino-timer.h>
#include <map>
#include <functional>
#include "relayDevice.hpp"
#include "mqttClient.hpp"
#include "helper.hpp"
#include "ImqttObserver.hpp"
#include <DHTesp.h>
#include "gasSensor.hpp"
#include "motionSensor.hpp"
#include "common.hpp"



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace app
{

struct persistantData
{
    float MQ2AnalogThrethhold = 1.0F;
    float MQ4AnalogThrethhold = 1.0F;
    bool MotionEnable = true;
};

class Application : public IObserver
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

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \param brokerIP
        //!
        void addClient(std::string brokerIP);

    private:

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \param topic
        //! \param message
        //!
        virtual void onMqttMessage(const std::string& topic, const std::string& message) override;

        //---------------------------------------------------------------------------
        //! \brief slits the topic to room and sensor
        //! \details if the topic is /home/Room/bathroom/light1 then \p room will be /home/Room/bathroom
        //! and \p sensor will be light1
        //! \param fullTopic    -- [in]  the full topic received by mqtt
        //! \param room         -- [out] the name of the room
        //! \param sensor       -- [out] the name of the sensor
        //!
        void splitRoomAndSensor(const std::string fullTopic, std::string& room, std::string& sensor);

        //---------------------------------------------------------------------------
        //! \brief read DHT11, MQ4 and MQ2 sensor readings and send it to all brokers on defined
        //! time interval (READINGSUPDATEINTERVAL in common.hpp)
        //!
        void updateSensorsReadings();

        //---------------------------------------------------------------------------
        //! \brief check the gas sensors values and update their values if needed
        //! 
        //!
        void checkHazeredSensors();

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        void checkMothion();

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \param command
        //! \param message
        //!
        void checkForConfigUpdate(const std::string& command, const std::string& message);

        //---------------------------------------------------------------------------
        //! \brief API to write data to EEPROM
        //!
        //! \tparam T       - type to be written
        //! \param address  - address to write at
        //! \param xdata    - data of type T to be written
        //!
        template<typename T>
        void writeToEEPROM(int address, T xdata);

        //---------------------------------------------------------------------------
        //! \brief API to read data from EEPROM
        //!
        //! \tparam T       - type to be read
        //! \param address  - address to read from
        //! \param xdata    - data of type T that will be read
        template<typename T>
        void readFromEEPROM(int address, T& xdata);

        // Data members
        // std::vector<std::pair<std::string, std::string>> m_brokerInitData;
        std::vector<std::string> m_brokerIps;

        // std::vector<mqtt::mqttClient> m_mqttClients;
        std::vector<std::shared_ptr<mqtt::mqttClient>> m_mqttClients;

        WiFiClient m_espClient;

        WiFiManager m_wifiManager;

        // source: https://chewett.co.uk/blog/1405/using-the-dht11-temperature-sensor-with-a-wemos-d1-mini-esp8266/
        DHTesp m_dhtSensor;

        float m_humidity;
        float m_temperature;

        unsigned long m_lastSentTime;

        Timer<10> m_timerTasks;

        std::function<bool(void*)> gpioCheckerTask;

        //---------------------------------------------------------------------------
        //! \brief object providing the reading for all Analog to digital channels
        //! to be used by other objects (sensor)
        //!
        Adafruit_ADS1115 m_ads;

        gasSensor m_MQ4Sensor;

        gasSensor m_MQ2Sensor;

        // map must use with ptr https://stackoverflow.com/a/2281473/6184259
        std::map<std::string, std::shared_ptr<bathRoom::bathRoomGPIO>> m_ControlGPIOsList;

        //---------------------------------------------------------------------------
        //! \brief motion sensor object
        //! 
        //!
        RCLWSensor m_RCWLSensor;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        persistantData m_persistantData;
};

}       //namespace app

