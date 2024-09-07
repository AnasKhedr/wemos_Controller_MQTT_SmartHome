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
#include "common/common.hpp"
#include "preferals/relay_device/relayDevice.hpp"

class RCLWSensor // : public ISimpleSensor
{
public:
    RCLWSensor(const uint8_t& sensorStatePin);

    virtual bool readDigitalValue(); // override;

    bool isNoMotionAfterInterval();

    void changeLightEnable(const bool& isEnabled);

    void controlLight();

    void setRelayControllerObject(std::shared_ptr<bathRoom::bathRoomGPIO>& object);
    // Data members

private:
    // Data members
    uint8_t m_sensorStatePin;

    bool m_isEnabled;

    bool didMotionSensorTurnOnLight;

    unsigned long m_motionDetectedTime;

    std::shared_ptr<bathRoom::bathRoomGPIO> m_lightControlPin;
};

#endif