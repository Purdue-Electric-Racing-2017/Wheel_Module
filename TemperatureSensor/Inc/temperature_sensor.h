#ifndef TEMPERATURE_SENSOR_H_
#define TEMPERATURE_SENSOR_H_

#include <math.h>



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
