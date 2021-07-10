//---------------------------------------------------------------------------
//! \file common.hpp
//! \authors Anas Khedr
//! \brief this file contains all the common defines
//! \version 0.1
//! \date 21-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

#ifndef __COMMON_HPP
#define __COMMON_HPP

//---------------------------------------------------------------------------
// DataDefins
//---------------------------------------------------------------------------


#define MQTTPORT 1883

// relay pins
#define MAINLIGHTPIN D6
#define WASHBASINELIGHTPIN D7
#define VENTILATORPIN D8
#define BUZZERCONTROLPIN 3      // RX
#define RCWLPIN D0

// buttons pins
#define MAINLIGHTBUTTONPIN D3
#define WASHBASINELIGHTBUTTONPIN D4
#define VENTILATORBUTTONPIN D5

// sensors pins
#define DHT11PIN 10      // SD3     --> if used D2, then the program will crash
// for some reason, the mapping is messed up in ESP but correct in arduino
#define ADSA1   1
#define ADSA2   2       //verified
#define ADSA3   3
#define ADSA0   0
constexpr float bitToVolt = (0.1875F/1000);

#define EEPROMSIZE  20
#define TESTBROKER "192.168.1.80"
#define PI_4_1 "192.168.1.100"
#define PI_4_2_kitchen "192.168.1.103"

#define MQ4DIGITALPIN D0

#define PESISTANTEEPROMIDX 0


#define ONESECOND 1000

#define MQTTLOOPRECONNECTRETRIES 10

//---------------------------------------------------------------------------
//! \brief 
//! \note   - auto will translate to char*
//!
constexpr auto bathRoomGeneralTopic("/home/Room/bathroom/");
constexpr auto bedRoomGeneralTopic("/home/Room/bedroom/");
constexpr auto livingRoomGeneralTopic("/home/Room/livingroom/");
constexpr auto receptionGeneralTopic("/home/Room/reception/");
constexpr auto kitchenGeneralTopic("/home/Room/kitchen/");

//---------------------------------------------------------------------------
//! \brief This topic will be used by the rest of the house(nymea) to
//! configure options in the device and how it behaves.
//!
constexpr auto bathRoomInputCommands("/home/Room/bathroom/command/");

//---------------------------------------------------------------------------
//! \brief This topic will be used by the bathroom controller to send
//! and update its data\sensors to the rest of the house
//!
constexpr auto bathRoomInfoData("/home/Room/bathroom/info/");

struct persistantData
{
    float MQ2AnalogThrethhold = 1.0F;
    float MQ4AnalogThrethhold = 1.0F;
    bool MotionEnable = true;
    unsigned long sensorsReadingsUpdateInterval = 5000;     /*< interval in ms after which the temperature and the Gas sensors reading will be sent to brokers*/
    unsigned long motionSensorLightActiveTime = 10000;
};

//---------------------------------------------------------------------------
//! \brief a stuct containing all the data that will be save to EEPROM
//! static to keep linker happy and avoid duplication
//!
static persistantData g_persistantData;

#endif