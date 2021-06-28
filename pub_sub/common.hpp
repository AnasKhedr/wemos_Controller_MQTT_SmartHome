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


#define MQTTPORT 1884

// relay pins
#define MAINLIGHTBUTTONPIN D6
#define WASHBASINELIGHTBUTTONPIN D7
#define VENTILATORBUTTONPIN D8
#define BUZZERCONTROLPIN 3     // RX

// buttons pins
#define MAINLIGHTPIN D3
#define WASHBASINELIGHTPIN D4
#define VENTILATORPIN D5

// sensors pins
#define DHT11PIN 10      // SD3     --> if used D2, then the program will crash
// for some reason, the mapping is messed up in ESP but correct in arduino
#define ADSA1   1
#define ADSA2   2       //verified
#define ADSA3   3
#define ADSA0   0
constexpr float bitToVolt = (0.1875F/1000);

#define TESTBROKER "192.168.1.80"

#define MQ4DIGITALPIN D0

//---------------------------------------------------------------------------
//! \brief interval in ms after which the temperature and the Gas sensors
//! reading will be sent to brokers
//!
#define INTERVAL 5000

//---------------------------------------------------------------------------
//! \brief 
//! \note   - auto will translate to char*
//!
constexpr auto bathRoomGeneralTopic("/home/Room/bathroom/");
constexpr auto bedRoomGeneralTopic("/home/Room/bedroom/");
constexpr auto livingRoomGeneralTopic("/home/Room/livingroom/");
constexpr auto receptionGeneralTopic("/home/Room/reception/");
constexpr auto kitchenGeneralTopic("/home/Room/kitchen/");



#endif