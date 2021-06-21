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
#define MAINLIGHTBUTTONPIN D5
#define WASHBASINELIGHTBUTTONPIN D6
#define VENTILATORBUTTONPIN D7

// buttons pins
#define MAINLIGHTPIN D1
#define WASHBASINELIGHTPIN D3
#define VENTILATORPIN D3

#define DHT11PIN D4

#define TESTBROKER "192.168.1.80"

#define MQ4DIGITALPIN D0
#define INTERVAL 10000

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