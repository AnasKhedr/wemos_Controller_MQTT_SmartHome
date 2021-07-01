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
    didMotionSensorTurnOnLight(false)
{
    Serial.printf("Motion Sensor Pin is set to: %d\n", sensorStatePin);
    pinMode(m_sensorStatePin, INPUT_PULLUP);
}

bool RCLWSensor::readDigitalValue()
{
    return digitalRead(m_sensorStatePin);
}

void RCLWSensor::changeLightEnable(bool isEnabled)
{
    m_isEnabled = isEnabled;
}

void RCLWSensor::setRelayControllerObject(std::shared_ptr<bathRoom::bathRoomGPIO>& object)
{
    m_lightControlPin = object;
}

void RCLWSensor::controlLight()
{
    if(m_isEnabled)
    {
        // if the sensor detets motion
        // and the light state is off
        // Serial.printf("Motion reading from Pin[%d]: %d\n", m_sensorStatePin, readDigitalValue());
        if(readDigitalValue() && (didMotionSensorTurnOnLight == false))
        {
            Serial.println("Motion detected turning on the Light");
            m_motionDetectedTime = millis();
            didMotionSensorTurnOnLight = true;

            m_lightControlPin->switchOn();
        }
        else
        {
            // make sure you only turn off the light if it was turned on by sensor
            // and time for turning off passed
            if(didMotionSensorTurnOnLight &&
                ((millis() - m_motionDetectedTime) > MOTIONSENSORONINTERVAL))
            {
                Serial.printf("Motion is no longer detected for %dms, turning off the light.\n",
                                MOTIONSENSORONINTERVAL);
                didMotionSensorTurnOnLight = false;
                m_lightControlPin->switchOff();
            }
        }
    }
}
