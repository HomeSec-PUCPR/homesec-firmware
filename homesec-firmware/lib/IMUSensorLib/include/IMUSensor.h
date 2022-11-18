/**
 * @file IMUSensor.h
 * @author Carlos Eduardo Marques Assunção Torres (carlos.torres@vido-la.com.br)
 * @brief Superclasse dos sensores IMU.
 * @version 0.1
 * @date 17-11-2021
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <Arduino.h>
#include <algorithm>
#include <list>
#include <math.h>
#include <vector>
#include <Wire.h>

#include "CircularBuffer.h"
#include "I2Cdev.h"
#include "IMUSensorStructs.h"

const int g_historySize = 100; // Tamanho do histórico de leituras do sensor.

/**
 * @brief Superclasse de sensores IMU
 * 
 */
class IMUSensor
{
protected:
    /**
     * @brief Inicializar o IMU.
     * 
     * @param wire Interface I2C.
     * @return true - Se o sensor iniciar corretamente.
     * @return false - Caso contrário.
     */
    bool begin(TwoWire &wire);

    /**
     * @brief Atualiza o buffer circular de leituras do 
     * sensor.
     */
    virtual void updateData() = 0;

    /**
     * @brief Adiciona novas informações no buffer histórico
     * de medidas do sensor.
     * @param measurement Medida a ser adicionada no buffer.
     */
    void addMeasurement(IMUAxisData_t measurement);

    /**
     * @brief Realiza a reinicialização do vetor de medidas
     * do sensor.
     */
    void resetMeasurements();

    /**
     * @brief Define o estado atual do equipamento de acordo
     * com as flags de estado.
     */
    void updateState();

    /**
     * @brief Realiza a detecção de movimento.
     * 
     */
    void detectMovement();

    /**
     * @brief Verifica se o dispositivo foi configurado.
     * 
     * @return true - Caso tenha sido configurado.
     * @return false - Caso contrário.
     */
    bool checkConfigurations();

    bool m_moving;                    // Flag de movimento.
    bool m_semaphoreInitialized;      // Flag que indica o funcionamento do semáforo.
    int m_readFrequency;              // Frequência da leitura do sensor.
    SemaphoreHandle_t m_imuSemaphore; // Semaforização de processos sensíveis.
    DeviceState_e m_devState;         // Estado atual do automóvel.

public:
    /**
     * @brief Realiza a calibração do sensor.
     * 
     * @return IMUOffsets_t - Offsets provenientes da calibração.
     */
    virtual IMUOffsets_t calibrate() = 0;

    /**
     * @brief Configurar o detector de movimento.
     * 
     * @param settings Configurações do detector de movimento.
     */
    void configureMovementDetection(IMUMovementSettings_t settings);
    
    /**
     * @brief Retornar a última leitura dos eixos do acelerômetro
     * e do giroscópio.
     * @return IMUAxisData_t - Eixos do acelerômetro e do giroscópio.
     */
    IMUAxisData_t getAxisData();    
    
    /**
     * @brief Iniciar a thread que realiza as medições.
     * 
     * @param frequency Frequência que a thread irá executar as medições (a cada x milissegundos).
     */
    virtual void start(int frequency) = 0;

    /**
     * @brief Retornar se a thread está rodando ou não.
     * 
     * @return true Caso esteja rodando.
     * @return false Caso contrário.
     */
    bool isRunning();

    /**
     * @brief Retorna o estado de movimento do equipamento.
     * 
     * @return true - Caso esteja em movimento.
     * @return false - Caso contrário.
     */
    bool getMovingState();
    
    /**
     * @brief Retorna os dados históricos da detecção de tombamento.
     * 
     * @param movementStruct Struct que armazenará os dados de movimento.
     */
    void getMovementData(IMUMovementData_t &movementStruct);

    /**
     * @brief Retorna o status atual do automóvel.
     * 
     * @return DeviceState_e - Estado do automóvel.
     */
    DeviceState_e getDevState();

    /**
     * @brief Template para o retorno dos Offsets ativos no sensor.
     * 
     * @return IMUOffsets_t - Struct contendo os offsets ativos do sensor.
     */
    virtual IMUOffsets_t getCurrentOffsets() = 0;

    /**
     * @brief Altera os offsets do dispositivo.
     * 
     * @param newOffsets Novos offsets que serão utilizados
     * pelo dispositivo
     */
    virtual void setOffsets(IMUOffsets_t newOffsets) = 0;

private: 
    IMUMovementSettings_t m_movementSettings; // Configurações para a detecção de movimento.
    IMUMovementData_t m_movementData;   // Dados de movimento.
    CircularBuffer <IMUAxisData_t, g_historySize> m_axisData; // Buffer circular com dados históricos das medidas do sensor.
};