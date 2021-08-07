//---------------------------------------------------------------------------
//! \file motionSensor.hpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 29-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

#ifndef _MOTIONSENSOR_HPP
#define _MOTIONSENSOR_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
// #include "ISimpleSensor.hpp"
#include "relayDevice.hpp"
#include "common.hpp"

class RCLWSensor// : public ISimpleSensor
{
    public:
        RCLWSensor(const uint8_t& sensorStatePin);

        virtual bool readDigitalValue();// override;

        bool isNoMotionAfterInterval();

        void changeLightEnable(const bool& isEnabled);

        void changeMotionSensorLightActiveTime(const unsigned long& motionSensorLightActiveTime);

        void controlLight(std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients);

        void setRelayControllerObject(std::shared_ptr<reception::receptionGPIO>& object);
        // Data members

    private:
        void updateMQTTClientsWithSensorState(std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients);

        // Data members
        uint8_t m_sensorStatePin;

        bool m_isEnabled;

        bool m_didMotionSensorTurnOnLight;

        unsigned long m_motionDetectedTime;

        unsigned long m_motionSensorLightActiveTime;

        std::shared_ptr<reception::receptionGPIO> m_lightControlPin;
};



#endif