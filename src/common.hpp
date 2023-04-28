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
// Data Define
//---------------------------------------------------------------------------

/// TODO: D3(GPIO0 pull up by a resistor), D4, D8 --> make them as an output pins to not messup bootmode.
// avoid using Tx (GPIO1) because it will mess up your serial.print()
// avoid using D8 as an input, it simple doesn't work
// relay pins
#define MAINLIGHTPIN D0
#define NEERLIGHTPIN D1
#define FARLIGHTPIN D2
#define DOORLIGHTPIN 3 // GPIO3 -> Rx
#define ABLEEKLIGHTPIN 1 // GPIO1 -> Tx

// buttons pins
#define MAINLIGHTBUTTONPIN D5
#define NEERLIGHTBUTTONPIN D6
#define FARBUTTONPIN D7
#define DOORBUTTONPIN D4
#define ABLEEKBUTTONPIN D3

// sensors pins
#define DHT11PIN 10 // SD3     --> if used D2, then the program will crash
// for some reason, the mapping is messed up in ESP but correct in arduino
#define ADSA1 1
#define ADSA2 2 // verified
#define ADSA3 3
#define ADSA0 0
constexpr float bitToVolt = (0.1875F / 1000);

#define EEPROMSIZE 20
#define TESTBROKER "192.168.1.80"
#define PI_4_1 "192.168.1.100"
#define PI_4_2_kitchen "192.168.1.103"
#define PI_3_reception "192.168.1.101"
#define MQTTPORT 1883
#define DEVICEIP IPAddress(192, 168, 1, 98)

#define PESISTANTEEPROMIDX 0

#define ONESECOND 1000
#define BUTTONDEBOUNCINGDELAY_MS 100

#define MQTTLOOPERCONNECTRETRIES 360 // esp will keep trying to reconnect for 30mins before resting
#define MQTTRECONNECTIONINTERVALS 10000 // 10 seconds
#define MQTTINITCONNECTRETRIES 5

#define CLIENTID "ESP8266Client-Office"
#define HOST_NAME "office"

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
constexpr auto officeInputCommands("/home/Room/office/command/");

//---------------------------------------------------------------------------
//! \brief This topic will be used by the bathroom controller to send
//! and update its data\sensors to the rest of the house
//!
constexpr auto officeInfoData("/home/Room/office/info/");

struct persistantData
{
    float MQ2AnalogThrethhold = 1.0F;
    float MQ4AnalogThrethhold = 1.0F;
    bool MotionEnable = true;
    unsigned long sensorsReadingsUpdateInterval =
        5000; /*< interval in ms after which the temperature and the Gas sensors reading will be sent to brokers*/
    unsigned long motionSensorLightActiveTime = 10000;
};

//---------------------------------------------------------------------------
//! \brief a stuct containing all the data that will be save to EEPROM
//! static to keep linker happy and avoid duplication
//!
static persistantData g_persistantData;

#endif