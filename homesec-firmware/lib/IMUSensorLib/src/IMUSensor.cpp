/**
 * @file IMUSensor.cpp
 * @author Carlos Eduardo Marques Assunção Torres (carlos.torres@vido-la.com.br)
 * @brief Arquivo de implementação das fundões da classe IMUSensor.
 * @version 0.1
 * @date 18-11-2021
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "IMUSensor.h"

uint16_t g_tippedCount = 0;   // Contador de quantas amostras foram analisadas para determinar tombamento.
uint16_t g_movementCount = 0; // Contador de quantas amostras foram analisadas para determinar movimento.
uint16_t g_stopCount = 0;     // Contador de quantas amostras foram analisadas para determinar parada.
uint16_t g_tamperCount = 0;   // Contador de quantas amostras foram analisadas para determinar tamper.
unsigned long g_firstTip = 0;       // Millis() em que ocorreu a primeira leitura de tombamento.
unsigned long g_firstMovement = 0;  // Millis() em que ocorreu a primeira leitura de movimento.
unsigned long g_firstStop = 0;      // Millis() em que ocorreu a primeira leitura de parada.
unsigned long g_firstTamper = 0;    // Millis() em que ocorreu a primeira leitura de tamper.
unsigned long g_firstMovingTip = 0; // Millis() em que é identificado um tombamento com movimento.

bool IMUSensor::begin(TwoWire &wire)
{
    m_imuSemaphore = xSemaphoreCreateMutex();
    m_semaphoreInitialized = m_imuSemaphore != NULL;
    
    return m_semaphoreInitialized;
}

void IMUSensor::configureMovementDetection(IMUMovementSettings_t settings)
{
    if(!m_semaphoreInitialized)
        return;

    xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
    m_movementSettings.MinimumSamples = settings.MinimumSamples;
    m_movementSettings.MovementInterval = settings.MovementInterval;
    xSemaphoreGive(m_imuSemaphore);
}

IMUAxisData_t IMUSensor::getAxisData()
{
    IMUAxisData_t lastData;

    if(!m_axisData.isEmpty() && m_semaphoreInitialized)
    {
        xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
        lastData = m_axisData.last();
        xSemaphoreGive(m_imuSemaphore);
    }

    return lastData;
}

bool IMUSensor::getMovingState()
{
    if(!m_semaphoreInitialized)
        return false;

    bool moving = false;

    xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
    moving = m_moving;
    xSemaphoreGive(m_imuSemaphore);

    return moving;
}

void IMUSensor::detectMovement()
{
    if (m_axisData.isEmpty() || !m_semaphoreInitialized)
        return;

    IMUAxisData_t lastData = getAxisData();

    double moduleAcc = sqrt(pow(lastData.Acc_X, 2) + pow(lastData.Acc_Y, 2) + pow(lastData.Acc_Z, 2));

    if (moduleAcc < (1 - m_movementSettings.MovementInterval) || moduleAcc > (1 + m_movementSettings.MovementInterval))
    {
        g_movementCount++;
    }
    else
    {
        g_movementCount = 0;
    }

    if (g_movementCount >= m_movementSettings.MinimumSamples)
    {
        g_stopCount = 0;

        xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
        m_moving = true;
        xSemaphoreGive(m_imuSemaphore);
    }

    if (moduleAcc > (1 - m_movementSettings.MovementInterval) && moduleAcc < (1 + m_movementSettings.MovementInterval))
    {
        g_stopCount++;
    }
    else
        g_stopCount = 0;

    if (g_stopCount >= 10)
    {
        g_movementCount = 0;
        xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
        m_moving = false;
        xSemaphoreGive(m_imuSemaphore);
    }
}

void IMUSensor::addMeasurement(IMUAxisData_t measurement)
{
    if (!m_semaphoreInitialized)
        return;

    xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
    m_axisData.push(measurement);
    xSemaphoreGive(m_imuSemaphore);
}

void IMUSensor::resetMeasurements()
{
    if (!m_semaphoreInitialized)
        return;

    xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
    m_axisData.clear();
    xSemaphoreGive(m_imuSemaphore);
}

void IMUSensor::getMovementData(IMUMovementData_t &movementStruct)
{
    if (!m_semaphoreInitialized)
        return;

    xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
    movementStruct.StartTime = m_movementData.StartTime;    
    xSemaphoreGive(m_imuSemaphore);
}

DeviceState_e IMUSensor::getDevState()
{
    DeviceState_e devState;

    if (m_semaphoreInitialized)
    {
        xSemaphoreTake(m_imuSemaphore, portMAX_DELAY);
        devState = m_devState;
        xSemaphoreGive(m_imuSemaphore);
    }

    return devState;
}

void IMUSensor::updateState()
{
    if (m_moving)
        m_devState = DeviceState_e::STATE_MOVING;
    else
        m_devState = DeviceState_e::STATE_STOPPED;
}