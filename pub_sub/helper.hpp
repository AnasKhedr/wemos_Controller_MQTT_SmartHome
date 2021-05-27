//---------------------------------------------------------------------------
//! \file helper.hpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 



#ifndef _HELPTER_HPP
#define _HELPTER_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Arduino.h>
#include <string>
// #include <pair

namespace helper
{
    // std::function<void (char* , byte* , unsigned int)> mqttCallback;

    // std::pair<std::string, double> mqttCallback(char *topic, byte *payload, unsigned int length)
    void mqttCallback(char *topic, byte *payload, unsigned int length);

    std::string random_string();


}   // namespace helper



#endif  //_HELPTER_HPP
