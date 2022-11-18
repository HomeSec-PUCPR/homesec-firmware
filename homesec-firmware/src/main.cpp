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

uint8_t g_calibrate = 1;

void setup() 
{
	Serial.begin(115200);

    if (MPU.begin(Wire))
    {
        Serial.printf("Nao foi possivel iniciar a MPU.\n");
    }
    else
    {
        Serial.printf("MPU inicializada com sucesso.");
    }

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
    
    MPU.start(1000);
}	

void loop() 
{

}