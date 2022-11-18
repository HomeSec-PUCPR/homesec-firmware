/**
 * @file ConnectionService.cpp
 * @author Carlos Eduardo Marques Assunção Torres (carlos.eduardo.qas@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ConnectionService.h"

void ConnectionServiceClass::begin()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin("", "");
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

ConnectionServiceClass ConnectionService;