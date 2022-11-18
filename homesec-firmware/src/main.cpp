/**
 * @file main.cpp
 * @author Carlos Eduardo Marques Assunção Torres (carlos.eduardo.qas@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include "IMUSensorLib.h"
#include "ConnectionService.h"

uint8_t g_calibrate = 1;
uint8_t g_ledBuiltIn = 2;

void setup() 
{
	Serial.begin(115200);

    if (MPU.begin(Wire))
    {
        Serial.printf("MPU inicializada com sucesso.");
    }

    IMUMovementSettings_t settings;

    settings.MinimumSamples = 1;
    settings.MovementInterval = 0.05;

    MPU.configureMovementDetection(settings);

    // Salvar offset na memória.

    if (g_calibrate)
    {
        IMUOffsets_t newOffsets = MPU.calibrate();
        Serial.printf("\n\nAccOffsets[x, y, z]: [%d, %d, %d]", newOffsets.XAccelOffset, 
                                                               newOffsets.YAccelOffset, 
                                                               newOffsets.ZAccelOffset);

        Serial.printf("\nGyroOffsets[x, y, z]: [%d, %d, %d]\n", newOffsets.XGyroOffset, 
                                                                newOffsets.YGyroOffset, 
                                                                newOffsets.ZGyroOffset);
    } 
    else
    {
        IMUOffsets_t currentOffsets = MPU.getCurrentOffsets();
        Serial.printf("\n\nAccOffsets[x, y, z]: [%d, %d, %d]", currentOffsets.XAccelOffset, 
                                                               currentOffsets.YAccelOffset, 
                                                               currentOffsets.ZAccelOffset);

        Serial.printf("\nGyroOffsets[x, y, z]: [%d, %d, %d]\n", currentOffsets.XGyroOffset, 
                                                                currentOffsets.YGyroOffset, 
                                                                currentOffsets.ZGyroOffset);
    }
    
    pinMode(g_ledBuiltIn, OUTPUT);

    MPU.start(10);
    ConnectionService.begin();
}	

void loop() 
{
    if (MPU.getDevState() == DeviceState_e::STATE_MOVING)
        digitalWrite(g_ledBuiltIn, HIGH);
    else
        digitalWrite(g_ledBuiltIn, LOW);

    delay(250);
}