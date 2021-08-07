//---------------------------------------------------------------------------
//! \file motionSensor.cpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 29-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "motionSensor.hpp"


//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

// RCLWSensor::RCLWSensor(const uint8_t& sensorStatePin, bathRoom::bathRoomGPIO& lightControlPin) :
RCLWSensor::RCLWSensor(const uint8_t& sensorStatePin) :
    m_sensorStatePin(sensorStatePin),
    // m_lightControlPin(lightControlPin),
    m_isEnabled(true),
    m_didMotionSensorTurnOnLight(false)
{
    Serial.printf("Motion Sensor Pin is set to: %d\n", sensorStatePin);
    pinMode(m_sensorStatePin, INPUT_PULLUP);
}

bool RCLWSensor::readDigitalValue()
{
    return digitalRead(m_sensorStatePin);
}

void RCLWSensor::changeLightEnable(const bool& isEnabled)
{
    m_isEnabled = isEnabled;
}

void RCLWSensor::changeMotionSensorLightActiveTime(const unsigned long& motionSensorLightActiveTime)
{
    m_motionSensorLightActiveTime = motionSensorLightActiveTime;
}

void RCLWSensor::setRelayControllerObject(std::shared_ptr<reception::receptionGPIO>& object)
{
    m_lightControlPin = object;
}

void RCLWSensor::controlLight(std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients)
{
    updateMQTTClientsWithSensorState(mqttClients);

    if(m_isEnabled)
    {
        // if the sensor detets motion
        // and the light state is off
        // Serial.printf("Motion reading from Pin[%d]: %d\n", m_sensorStatePin, readDigitalValue());
        if(readDigitalValue() && (m_didMotionSensorTurnOnLight == false))
        {
            Serial.println("Motion detected turning on the Light");
            m_motionDetectedTime = millis();
            m_didMotionSensorTurnOnLight = true;

            m_lightControlPin->switchOn();
        }
        else
        {
            // make sure you only turn off the light if it was turned on by sensor
            // and time for turning off passed
            if(m_didMotionSensorTurnOnLight &&
                ((millis() - m_motionDetectedTime) > m_motionSensorLightActiveTime))
            {
                Serial.printf("Motion is no longer detected for %dms, turning off the light.\n",
                                m_motionSensorLightActiveTime);
                m_didMotionSensorTurnOnLight = false;
                m_lightControlPin->switchOff();
            }
        }
    }
}

void RCLWSensor::updateMQTTClientsWithSensorState(std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients)
{
    static bool c_lastState, c_currentState;

    // save last state and update current state
    c_lastState = c_currentState;
    c_currentState = readDigitalValue();

    // if state hasn't chaged since last read then do nothing
    if(c_currentState == c_lastState)
    {
        return;
    }
    else
    {
        for(auto&& oneClient : mqttClients)
        {
            oneClient->publish(std::string(receptionInfoData)+"mothionState", std::to_string(c_currentState));
        }
    }
}
