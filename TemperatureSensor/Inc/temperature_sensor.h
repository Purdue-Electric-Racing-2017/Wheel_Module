#ifndef TEMPERATURE_SENSOR_H_
#define TEMPERATURE_SENSOR_H_

#include <stdio.h>
#include <math.h>
#include "main.h"
#include "stm32f4xx_hal.h"


HAL_StatusTypeDef temperatureSensorInitialize();
static HAL_StatusTypeDef temperatureSensorEEPROMDataProcessing();
static void temperatureSensorInitEEPROMConstants();
static HAL_StatusTypeDef temperatureSensorWriteOscillatorTrimValue();
static HAL_StatusTypeDef temperatureSensorWriteConfigurationRegister();

HAL_StatusTypeDef temperatureSensorGetTemp();
static HAL_StatusTypeDef temperatureSensorPTATDataHandler();
static HAL_StatusTypeDef temperatureSensorIRDataHandler();
static HAL_StatusTypeDef temperatureSensorCompensationPixelDataHandler();
static void temperatureSensorTempCalc();
static float temperatureSensorTaCalc();
static void temperatureSensorToCalc(float Ta);

#endif /* TEMPERATURE_SENSOR_H_ */
