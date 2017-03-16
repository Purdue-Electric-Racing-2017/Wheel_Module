/***************************************************************************
*
*     Function Information
*
*     Name of Function:  CheckDeviceReadiness
*
*     Programmer's Name:  Matthew Gaydos
*
*     Function Return Type:  HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.
*
*      Global Dependents:
*	    1.	I2C_HandleTypeDef * hi2c_t
*       	This parameter is configured in the I2C Config calls which is accomplished in
*       	the pre-generated code.  It is a struct that contains important information
*       	for the successful transmission of information via I2C.
*
*     Function Description:
*     		This function checks to ensure that the Melexis MLX90621 IR Array is ready
*     		to communicate with the Wheel Module's MCU via I2C.
*
***************************************************************************/


#include "temperature_sensor.h"
#include "main.h"
#include "stm32f4xx_hal.h"


#define EEPROM_SLAVE_ADDRESS (0x50 << 1)
#define EEPROM_INITIAL_DATA_ADDRESS 0x00
#define NUMBER_OF_TRIALS 5
#define TIMEOUT_DURATION 1000  //1 second
#define MLX_MCU_SLAVE_ADDRESS (0x60 << 1)
#define OSCILLATOR_TRIM_VALUE_START_ADDRESS 0x93
#define CONFIGURATION_REGISTER_START_ADDRESS 0x92
#define PTAT_READ_START_ADDRESS 0x40
#define IR_READ_START_ADDRESS 0x00
#define COMPENSATION_PIXEL_START_ADDRESS 0x41


extern I2C_HandleTypeDef hi2c_t;
extern WheelModule_t gWheelData;
uint8_t gpData_PTAT_Request[4];
uint16_t gPTAT;
uint8_t gpData_IR_Request[4];
uint16_t gIR[64];
uint8_t gpData_Compensation_Pixel_Request[4];
uint16_t gCompensation_Pixel;
int gV_th_25;
uint8_t gConfig_Reg_54;
int gK_t1;
int gK_t2;
int gA_i[64];
int gB_i[64];
int gTGC;
int gEmissivity;
int gKsta;
int gAlpha[64];
int gAlpha_cp;


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorInitialize
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*
*     Function Description:
*     Performs necessary actions to get MLX Sensor ready to begin taking measurements
*     by checking the device is ready to communicate, initializing the EEPROM data from
*     the sensor to our MCU, writing the Oscillator Trim Value, and setting the
*     configuration register.  This function returns a HAL_StatusTypeDef that provides
*     information regarding the status of the initialization.
*
***************************************************************************/
HAL_StatusTypeDef temperatureSensorInitialize()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  /* --------- I2C COMMUNICATION CHECKS --------- */
  if(HAL_OK != HAL_I2C_IsDeviceReady(&hi2c_t, EEPROM_SLAVE_ADDRESS, NUMBER_OF_TRIALS, TIMEOUT_DURATION))
  {
    //probably want to try recalling the "MX_I2C1_Init()" function here
	return_status = HAL_ERROR;
  }

  /* --------- EEPROM HANDLING --------- */
  if(HAL_OK != temperatureSensorEEPROMDataProcessing())
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  /* --------- WRITING OSCILLATOR TRIM VALUE --------- */
  if(HAL_OK != temperatureSensorWriteOscillatorTrimValue())
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  /* --------- WRITING CONFIGURATION REGISTER --------- */
  if(HAL_OK != temperatureSensorWriteConfigurationRegister())
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  /* --------- I2C TRANSMISSION INITIALIZATIONS --------- */
  //PTAT
  gpData_PTAT_Request[0] = 0x02;  					//command
  gpData_PTAT_Request[1] = PTAT_READ_START_ADDRESS;	//start address
  gpData_PTAT_Request[2] = 0x00; 					//address step
  gpData_PTAT_Request[3] = 0x01;  					//number of reads

  //IR
  gpData_IR_Request[0] = 0x02; 						//command
  gpData_IR_Request[1] = IR_READ_START_ADDRESS; 	//start address
  gpData_IR_Request[2] = 0x01; 						//address step
  gpData_IR_Request[3] = 0x40; 	 					//number of reads

  //Compensation Pixel
  gpData_Compensation_Pixel_Request[0] = 0x02;  							//command
  gpData_Compensation_Pixel_Request[1] = COMPENSATION_PIXEL_START_ADDRESS;  //start address
  gpData_Compensation_Pixel_Request[2] = 0x00;  							//address step
  gpData_Compensation_Pixel_Request[3] = 0x01;  							//number of reads



  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorEEPROMDataProcessing
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*
*     Function Description:
*     Sends request to receive EEPROM dump, accepts EEPROM dump, and then stores
*     select constants into their respective global variables for ease of
*     temperature calculation later on.
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorEEPROMDataProcessing()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  //EEPROM Dump Request
  uint8_t * pData_EEPROM_Dump_Request;
  *pData_EEPROM_Dump_Request = 0x00;
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, EEPROM_SLAVE_ADDRESS, EEPROM_INITIAL_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT, pData_EEPROM_Dump_Request, 0, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //EEPROM Dump Receive
  if(HAL_OK != HAL_I2C_Mem_Read(&hi2c_t, EEPROM_SLAVE_ADDRESS, EEPROM_INITIAL_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT, gEEPROM, sizeof(gEEPROM)))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //Initialize EEPROM Constants As Global Variables
  temperatureSensorInitEEPROMConstants();


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorInitEEPROMConstants
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. gEERPOM
*	   2. gConfig_Reg_54
*	   3. gV_th_25
*	   4. gK_t1
*	   5. gK_t2
*
*     Function Description:
*     Saves EEPROM constants to global variables for ease of access while preforming
*     calculations to find temperatures
*
***************************************************************************/
static void temperatureSensorInitEEPROMConstants()
{
  /* --------- gConfig_Reg_54 --------- */
  gConfig_Reg_54 = gEEPROM[0xF5];
  uint8_t config_reg_54_mask = 0b00110000;
  gConfig_Reg_54 = gConfig_Reg_54 & config_reg_54_mask;
  gConfig_Reg_54 = gConfig_Reg_54 >> 4;


  /* --------- gV_th_25 --------- */
  int V_th_l = gEEPROM[0xDA];
  int V_th_h = gEEPROM[0xDB];
  gV_th_25 = 256 * V_th_h + V_th_l;
  if(gV_th_25 > 32767)
  {
    gV_th_25 -= 65536;
  }
  gV_th_25 = gV_th_25 / pow(2, 3 - gConfig_Reg_54);


  /* --------- gK_t1 --------- */
  unsigned int K_scale = gEEPROM[0xD2];
  uint8_t K_t1_scale_mask = 0b11110000;
  int K_t1_scale = (K_scale & K_t1_scale_mask) >> 4;
  int K_t1_l = gEEPROM[0xDC];
  int K_t1_h = gEEPROM[0xDD];
  gK_t1 = 256 * K_t1_h + K_t1_l;
  if(gK_t1 > 32767)
  {
    gK_t1 -= 65536;
  }
  gK_t1 = gK_t1 / (pow(2, K_t1_scale) * pow(2, 3 - gConfig_Reg_54));


  /* --------- gK_t2 --------- */
  uint8_t K_t2_scale_mask = 0b00001111;
  int K_t2_scale = K_scale & K_t2_scale_mask;
  int K_t2_l = gEEPROM[0xDE];
  int K_t2_h = gEEPROM[0xDF];
  gK_t2 = 256 * K_t2_h + K_t2_l;
  if(gK_t2 > 32767)
  {
    gK_t2 -= 65536;
  }
  gK_t2 = gK_t2 / (pow(2, K_t2_scale + 10) * pow(2, 3 - gConfig_Reg_54));


  /* --------- gA_i --------- */
  //A_common
  uint8_t A_common_l = gEEPROM[0xD3];
  uint8_t A_common_h = gEEPROM[0xD4];
  int A_common = 256 * A_common_h + A_common_l;
  if(A_common > 32767)
  {
    A_common -= 65536;
  }
  //delta_A_i
  int delta_A_i[64];
  int B_i[64];
  int Ai_loop_control;
  for(Ai_loop_control = 0; Ai_loop_control < 64; Ai_loop_control++)
  {
	delta_A_i[Ai_loop_control] = gEEPROM[Ai_loop_control];
  }
  //delta_A_i_scale
  int delta_A_i_scale = gEEPROM[0xD9];
  uint8_t delta_A_i_scale_mask = 0b11110000;
  delta_A_i_scale = (delta_A_i_scale & delta_A_i_scale_mask) >> 4;
  //gA_i
  int gA_i_loop_control;
  for(gA_i_loop_control = 0; gA_i_loop_control < 64; gA_i_loop_control++)
  {
    gA_i[gA_i_loop_control] = (A_common + delta_A_i[gA_i_loop_control] * pow(2, delta_A_i_scale)) / pow(2, 3 - gConfig_Reg_54);
  }


  /* --------- gB_i --------- */
  //B_i
  int B_i[64];
  int Bi_loop_control;
  for(Bi_loop_control = 0; Bi_loop_control < 64; Bi_loop_control++)
  {
    B_i[Bi_loop_control] = gEEPROM[0x40 + Bi_loop_control];
    if(B_i[Bi_loop_control] > 127)
    {
      B_i[Bi_loop_control] -= 256;
    }
  }
  //B_i_scale
  int B_i_scale = gEEPROM[0xD9];
  uint8_t B_i_scale_mask = 0b00001111;
  B_i_scale = B_i_scale & B_i_scale_mask;
  //gB_i
  int gB_i_loop_control;
  for(gB_i_loop_control = 0; gB_i_loop_control < 64; gB_i_loop_control++)
  {
    gB_i[gB_i_loop_control] = B_i[gB_i_loop_control] / (pow(2, B_i_scale_mask) * pow(2, 3 - gConfig_Reg_54));
  }


  /* --------- gTGC --------- */
  gTGC = gEEPROM[0xD8] / 32;


  /* --------- gEmissivity --------- */
  int emissivity_l = gEEPROM[0xE4];
  int emissivity_h = gEEPROM[0xE5];
  gEmissivity = (256 * emissivity_h + emissivity_l) / 32768;


  /* --------- gKsta --------- */
  gKsta = (256 * gEEPROM[0xE7] + gEEPROM[0xE6]) / pow(2, 20);


  /* --------- gAlpha --------- */
  int gAlpha_loop_control;
  for(gAlpha_loop_control = 0; gAlpha_loop_control < 64; gAlpha_loop_control++)
  {
    gAlpha[gAlpha_loop_control] = (((256 * gEEPROM[0xE1] + gEEPROM[0xE0]) / pow(2, gEEPROM[0xE2])) + (gEEPROM[0x80 + gAlpha_loop_control] / pow(2, gEEPROM[0xE3]))) / pow(2, 3 - gConfig_Reg_54);
  }


  /* --------- gAlpha_cp --------- */
  gAlpha_cp = (256 * gEEPROM[0xD7] + gEEPROM[0xD6]) / (pow(2, gEEPROM[0xE2]) * pow(2, 3 - gConfig_Reg_54));

}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorWriteOscillatorTrimValue
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*
*     Function Description:
*     Writes to the slave MCU the Oscillator Trim Value from EEPROM
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorWriteOscillatorTrimValue()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  //Write Oscillator Trim Value
  uint8_t pData_Write_Oscillator_Trim_Value[5];
  pData_Write_Oscillator_Trim_Value[0] = 0x04;						//command
  pData_Write_Oscillator_Trim_Value[1] = gEEPROM[0xF7] - 0xAA;  	//LSB Check
  pData_Write_Oscillator_Trim_Value[2] = gEEPROM[0xF7];				//LSB
  pData_Write_Oscillator_Trim_Value[3] = 0x00 - 0xAA;				//MSB Check
  pData_Write_Oscillator_Trim_Value[4] = 0x00;						//MSB
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, OSCILLATOR_TRIM_VALUE_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, pData_Write_Oscillator_Trim_Value, 5, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorWriteConfigurationRegister
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*
*     Function Description:
*     Writes the Configuration Register from EEPROM to the Slave MCU
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorWriteConfigurationRegister()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  //Write Configuration Register
  uint8_t pData_Write_Configuration_Register[5];
  pData_Write_Configuration_Register[0] = 0x03;						//command
  pData_Write_Configuration_Register[1] = gEEPROM[0xF5] - 0x55;  	//LSB Check
  pData_Write_Configuration_Register[2] = gEEPROM[0xF5];  			//LSB
  pData_Write_Configuration_Register[3] = gEEPROM[0xF6] - 0x55;  	//MSB Check
  pData_Write_Configuration_Register[4] = gEEPROM[0xF6];  			//MSB
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, CONFIGURATION_REGISTER_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, pData_Write_Configuration_Register, 5, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorGetTemp
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*
*     Function Description:
*     Finds PTAT data, IR data, compensation data, and calculates the final temperatures
*
***************************************************************************/
HAL_StatusTypeDef temperatureSensorGetTemp()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  /* --------- PTAT DATA ACQUISITION --------- */
  if(HAL_OK != temperatureSensorPTATDataHandler())
  {
    //ERROR HANDLER
	return_status = HAL_ERROR;
  }


  /* --------- IR PIXEL ARRAY DATA ACQUISITION --------- */
  if(HAL_OK != temperatureSensorIRDataHandler())
  {
    //ERROR HANDLER
    return_status = HAL_ERROR;
  }


  /* --------- COMPENSATION PIXEL DATA ACQUISITION --------- */
  if(HAL_OK != temperatureSensorCompensationPixelDataHandler())
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  /* --------- TEMPERATURE CALCULATION --------- */
  temperatureSensorTempCalc();



  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorPTATDataHandler
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*	   2. uint8_t gpData_PTAT_Request[4]
*	   3. uint16_t gPTAT
*
*     Function Description:
*     Requests PTAT information, receives it, and then concatenates it
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorPTATDataHandler()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  //PTAT Request
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, PTAT_READ_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, gpData_PTAT_Request, 4, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //PTAT Receive
  uint8_t data_PTAT[2];  //needs to be an array of size two to accept LSB and MSB
  if(HAL_OK != HAL_I2C_Mem_Read(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, PTAT_READ_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, data_PTAT, sizeof(data_PTAT), TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //PTAT Data Concatenation
  gPTAT = data_PTAT[1] << 8 | data_PTAT[0];


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorIRDataHandler
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*	   2. uint8_t gpData_IR_Request[4]
*	   3. uint16_t gIR[64]
*
*     Function Description:
*     Requests IR information, receives it, and then concatenates it
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorIRDataHandler()
{
  HAL_StatusTypeDef return_status;


  //IR Request
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, IR_READ_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, gpData_IR_Request, 4, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //IR Receive
  uint8_t data_IR[128];  //64 pixels, each with an LSB and MSB
  if(HAL_OK != HAL_I2C_Mem_Read(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, IR_READ_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, data_IR, sizeof(data_IR), TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //IR Data Concatenation
  int IR_concatenation_loop_control;
  for(IR_concatenation_loop_control = 0; IR_concatenation_loop_control < 64; IR_concatenation_loop_control += 2)
  {
    gIR[IR_concatenation_loop_control] = data_IR[IR_concatenation_loop_control + 1] << 8 | data_IR[IR_concatenation_loop_control];
  }


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorCompensationPixelDataHandler
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: HAL_StatusTypeDef
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. HAL_HandleTypeDef hi2c_t
*	   2. uint8_t gpData_Compensation_Pixel_Request[4]
*	   3. uint16_t gCompensation_Pixel
*
*     Function Description:
*     Requests IR information, receives it, and then concatenates it
*
***************************************************************************/
static HAL_StatusTypeDef temperatureSensorCompensationPixelDataHandler()
{
  HAL_StatusTypeDef return_status = HAL_OK;


  //Compensation Pixel Request
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, COMPENSATION_PIXEL_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, gpData_Compensation_Pixel_Request, 4, TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //Compensation Pixel Receive
  uint8_t data_Compensation_Pixel[2];
  if(HAL_OK != HAL_I2C_Mem_Read(&hi2c_t, MLX_MCU_SLAVE_ADDRESS, COMPENSATION_PIXEL_START_ADDRESS, I2C_MEMADD_SIZE_16BIT, data_Compensation_Pixel, sizeof(data_Compensation_Pixel), TIMEOUT_DURATION))
  {
    //ERROR HANDLING
	return_status = HAL_ERROR;
  }


  //Compensation Pixel Data Concatenation
  gCompensation_Pixel = data_Compensation_Pixel[1] << 8 | data_Compensation_Pixel[0];


  return return_status;
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorTempCalc
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. uint16_t gPTAT
*	   2. uint16_t gCompensation_Pixel
*	   3. uint16_t gIR[64]
*	   4. WheelModule_t gWheelData
*
*     Function Description:
*     Calculates each pixel's respective perceived temperature
*
***************************************************************************/
static void temperatureSensorTempCalc()
{
  float Ta = temperatureSensorTaCalc();
  temperatureSensorToCalc(Ta);
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorTaCalc
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: float
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. gV_th_25
*	   2. gK_T1
*	   3. gK_T2
*	   4. gPTAT
*
*     Function Description:
*     Finds the Ta (absolute temperature of the chip/sensor temperature), as it
*     is used for additional calculations.  Calculations taken from the MLX 90621
*     data-sheet on page 14.
*
***************************************************************************/
static float temperatureSensorTaCalc()
{
  return ((-gK_t1 + pow(pow(gK_t2, 2) - 4 * (gV_th_25 - gPTAT))) / (2 * gK_t2) + 25);
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: temperatureSensorToCalc
*
*     Programmer's Name:  Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.  NONE
*
*      Global Dependents:
*	   1. gIR
*	   2. gA_i
*	   3. gB_i
*	   4. gCompensation_Pixel
*	   5. gTGC
*	   6. gEmissivity
*	   7. gKsta
*	   8. gAlpha
*	   9. gAlpha_cp
*
*     Function Description:
*     Calculates the To(the temperature seen by each specific pixel).  The calculations
*     are taken from the MLX 90621 data-sheet on pages 17-19.  Although I do not know the
*     exact model we are using (BAA, BAB, BAD), it is apparent to me based off of the
*     dimensions given for the BAA on the data-sheet, mine is not the BAA.  Thus, Ks4 is
*     0 for both the BAD and BAB, so I will treat Ks4 as 0, and it will not be put in the
*     calculation as a method of simplification.
*
***************************************************************************/
static void temperatureSensorToCalc(float Ta)
{
  /* --------- T_ak4 CALCULATION --------- */
  double T_ak4 = pow(Ta + 273.15, 4);


  /* --------- V_IR_Comp CALCULATION --------- */
  int v_ir_offset_comp[64];
  int v_ir_tgc_comp[64];
  int v_ir_comp[64];
  int Ta0 = 25;
  int v_ir_comp_loop_control;
  for(v_ir_comp_loop_control = 0; v_ir_comp_loop_control < 64; v_ir_comp_loop_control++)
  {
    v_ir_offset_comp[v_ir_comp_loop_control] = gIR[v_ir_comp_loop_control] - (gA_i  + gB_i * (Ta - Ta0));
    v_ir_tgc_comp[v_ir_comp_loop_control] = v_ir_offset_comp[v_ir_comp_loop_control] - gTGC * gCompensation_Pixel;
    v_ir_comp[v_ir_comp_loop_control] = v_ir_tgc_comp / gEmissivity;
  }


  /* --------- ALPHA_COMP CALCULATION --------- */
  int alpha_comp[64];
  int alpha_comp_loop_control;
  for(alpha_comp_loop_control = 0; alpha_comp_loop_control < 64; alpha_comp_loop_control++)
  {
    alpha_comp[alpha_comp_loop_control] = (1 + gKsta * (Ta - Ta0)) * (gAlpha[alpha_comp_loop_control] - gTGC * gAlpha_cp);
  }


  /* --------- To CALCULATION --------- */
  int To_loop_control;
  for(To_loop_control = 0; To_loop_control < 64; To_loop_control++)
  {
    gWheelData.temperature[To_loop_control] = pow(v_ir_comp[To_loop_control] / alpha_comp[To_loop_control] + T_ak4, .25) - 273.15;
  }
}
