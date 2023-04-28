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
#include "ImqttObserver.hpp"
#include "common.hpp"
#include "gasSensor.hpp"
#include "helper.hpp"
#include "motionSensor.hpp"
#include "mqttClient.hpp"
#include "relayDevice.hpp"
#include <Adafruit_ADS1015.h> // https://github.com/adafruit/Adafruit_ADS1X15 version 1.1.1
#include <Arduino.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
#include <functional>
#include <mDNSResolver.h>
#include <unordered_map>

#include <AsyncMqttClient.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace app
{
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
    //! \brief add the broker hostname, the hostname will be resolved into the ip
    //!
    //! \param[in] brokerHostname   - the hostname of the mqtt broker that we want to connect to.
    //!
    void addBroker(const std::string& brokerHostname);

    void setupBrokers();

    std::vector<std::string> m_resolveFailedHostnames;

private:
    void initWifi();

    void initGpioDevices();

    void initRemoteDebug();

    void setupBroker(const std::string& hostname, const std::string& ip);

    //---------------------------------------------------------------------------
    //! \brief
    //!
    //! \param topic
    //! \param message
    //!
    virtual void onMqttMessage(const std::string& topic, const std::string& message) override;

    //---------------------------------------------------------------------------
    //! \brief
    //!
    //! \param brokerIP
    //!
    void onMqttDisconnected(const std::string& brokerIP) override;

    //---------------------------------------------------------------------------
    //! \brief slits the topic to room and sensor
    //! \details if the topic is /home/Room/office/light1 then \p room will be /home/Room/office
    //! and \p sensor will be light1
    //! \param fullTopic    -- [in]  the full topic received by mqtt
    //! \param room         -- [out] the name of the room
    //! \param sensor       -- [out] the name of the sensor
    //!
    void splitRoomAndSensor(const std::string fullTopic, std::string& room, std::string& sensor);

    //---------------------------------------------------------------------------
    //! \brief read DHT11, MQ4 and MQ2 sensor readings and send it to all brokers on defined
    //! time interval (g_persistantData.sensorsReadingsUpdateInterval in common.hpp)
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
    //! \brief function to loop over #m_brokerHostnameIPMap and try to connect to them
    //!
    //! \return true    - if connected to all m_brokerHostnameIPMap successfully
    //! \return false   - if failed to connect to any of the broker ip in m_brokerHostnameIPMap
    //!
    bool connectToBroker(const std::string& mqttBrokerHostname);

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

    //---------------------------------------------------------------------------
    //! \brief convert \p hostname of a machine on the local network to ip
    //! \todo this function takes too much time and could block buttons
    //! \todo the hostname of raspberry pi maybe lost after rpi reboot(this is a raspberry pi issue)
    //! \param[in] hostname - the hostname to be resolved to ip
    //! \return IPAddress   - the ip of the \p hostname
    //!
    IPAddress hostnameToIP(const std::string& hostname);

    // Data members
    // std::vector<std::pair<std::string, std::string>> m_brokerInitData;
    //---------------------------------------------------------------------------
    //! \brief brokers map
    //! first is the hostname and second is the ip
    //!
    std::unordered_map<std::string, std::string> m_brokerHostnameIPMap;

    // std::vector<mqtt::mqttClient> m_mqttClients;
    std::vector<std::unique_ptr<mqtt::mqttClient>> m_mqttClients;

    WiFiClient m_espClient;

    WiFiManager m_wifiManager;

    // source: https://chewett.co.uk/blog/1405/using-the-dht11-temperature-sensor-with-a-wemos-d1-mini-esp8266/
    DHTesp m_dhtSensor;

    float m_humidity;
    float m_temperature;

    bool m_brokerStatus = true;

    unsigned long m_lastSentTime;

    Timer<10> m_timerTasks;

    std::function<bool(void*)> gpioCheckerTask;

    //---------------------------------------------------------------------------
    //! \brief object providing the reading for all Analog to digital channels
    //! to be used by other objects (sensor)
    //!
    // Adafruit_ADS1115 m_ads;

    // gasSensor m_MQ4Sensor;

    // gasSensor m_MQ2Sensor;

    // map must use with ptr https://stackoverflow.com/a/2281473/6184259
    std::unordered_map<std::string, std::unique_ptr<office::officeGPIO>> m_ControlGPIOsList;

    WiFiUDP m_udp;

    // docs: https://github.com/lbussy/mDNSResolver/blob/main/examples/mDNSResolver.ino
    mDNSResolver::Resolver m_resolver;
};

} // namespace app
