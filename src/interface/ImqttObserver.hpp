//! \file ImqttObserver.hpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 30-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!

#ifndef _MQTTOBSERVER_HPP
#define _MQTTOBSERVER_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <string>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

class IObserver
{
public:
    IObserver() = default;
    // Data members

    virtual void onMqttMessage(const std::string& topic, const std::string& message) = 0;

private:
    // Data members
};

#endif // _MQTTOBSERVER_HPP