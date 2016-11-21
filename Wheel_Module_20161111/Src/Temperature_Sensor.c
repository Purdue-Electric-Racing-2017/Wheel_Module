/***************************************************************************
*
*     File Information
*
*     Name of File: Temperature_Sensor.c
*
*     Authors (Include Email):
*       1. Matt Flanagan mflanag@purdue.edu
*       2. Matt Gaydos gaydos@purdue.edu
*
*     File dependents: (header files, flow charts, referenced documentation)
*       1. Temperature_Sensor.h
*       2. Temperature Sensor Flow Chart
*       3. Josh's can function thing
*
*     File Description: (Describe overall what part of the car the file
*       addresses as well as any communication protocols it uses.
*       In this program it will initialize the melexis mlx90621. Then will lay
*       idle until it is called with a read message from the micro controller every
*       .1 seconds. Once this message is received it will send an I2C message to the
*       temperature sensor telling it to get a reading which will then send back to the
*       Micro Controller and then write it to a corresponding register address. Then
*       the micro controller will take this data and then convert it to CAN and then
*       finally send it to the Main Module.
*
***************************************************************************/

/***************************************************************************
*
*     Function Information
*     Name of Function: init_temp_sensor_16bit
*
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. uint16_t init_value the 16 bit value that will be used to configure the temperature sensor
*
*      Global Dependents:
*	   1. None
*
*     Function Description: given a hardcoded initialization value write it to the 0x92 register in order to set up the temperature sensor
*
***************************************************************************/
void init_temp_sensor_16bit(uint16_t init_value) {
	//Initialize EEProm


	//make in step mode
	//128Hz refresh rate
	//normal operation
	//IR running
	//16bit resolution so that it is easy
	//0100011001010111
	//write to 0x92

	//------------error=====================
	//error possibility time out


	//eeprom


}

/***************************************************************************
*     Function Information
*     Name of Function: temp_main
*
*     Programmer's Name: Matt Gaydos
*     					 Matt Flanagan
*
*     Function Return Type: int value that will correspond to the success or lack of success of this function
*
*     Parameters (list data type, name, and comment one per line):
*       1.None
*
*      Global Dependents:
*	   1.None
*
*     Function Description: This is the main temperature function that is called by the actual main function.
*     Its purpose is to call other functions in this file in order to get a I2C message store it in the micro
*     and then take that and send a can message to the micro controller.
*
***************************************************************************/
int temp_main() {
	//check to see if any errors are happening

	//send an I2C message to the temperature sensor to start a measurement


	//this function will be called immediately after the temperature sensor is supposed to measure
	//and then given a timeout value already will wait until measuring finished and then write to
	//address in the MC
	//HAL_FMPI2C_Mem_Write()

	//take the data stored at address and call can function

}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: init_temp_sensor_trim
*
*     Programmer's Name: Matt Gaydos
*     					 Matt Flanagan
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. void * trim register
*
*      Global Dependents:
*	   1.None
*
*     Function Description:given a trim value write it to the 0x93 register
*
***************************************************************************/
void init_temp_sensor_trim(void * trim_register) {
	//initialize trimming value
	//timeout value
	//read trimming value from eeprom

	//write trim to the 0x93 register
	//write with I2C

}


/***************************************************************************
*     Function Information
*     Name of Function: read_trim_value
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type:	uint8_t the byte of the trimming value
*
*     Parameters (list data type, name, and comment one per line):
*       1. void * trim_location register where it is stored
*
*      Global Dependents:
*	   1. NONE
*
*     Function Description: reads the value stored at address 0xF7 which should be the 8 bits for the trimming value
	  0xF7 is in EEProm
*
***************************************************************************/
uint8_t read_trim_value(void * trim_location) {
	//casts it to the appropriate value then returns what is stored at that value
	uint8_t * address = (uint8_t *) trim_location;
	return *address;
}





//MAY NEED TO JUST READ EEPROM (COULD BE INITIALIZED FOR US!!)
/*//initialize all of the eeprom registers with the proper coefficients and such
void initialize_temperature_sensor_eeprom() {
	//eeprom addresses
	//I2C address is at 0x50
		//0xDA	Vth_L	0x20	stores the low temp
		//set to 10 degrees C

		//0xDB	Vth_H	0x64	stores the high temp
		//set at 85 degrees celsius

		//0xDC	Kt1_L	0x89


		//0xDD	Kt1_H	0x55

		//0xDE	Kt2_L	0x7E

		//0xDF	Kt2_H	0x5E

		//0xD2	Kt_scale	0x8B



}*/



//=============================================================
//MAYBE NOT NEEDED


//some of the registers in eeprom need to be twos complement
//in order to do this you have to xor the given byte with 0xFF
//and then add one (essentially this gets the negative value of
//the byte given
/***************************************************************************
*
*     Function Information
*
*     Name of Function:
*
*     Programmer's Name:
*
*     Function Return Type:
*
*     Parameters (list data type, name, and comment one per line):
*       1.
*
*      Global Dependents:
*	   1.
*
*     Function Description:
*
***************************************************************************/
/*int twos_complement(int x) {
	int bit_mask = 0xFF;
	return ((x ^ bit_mask) + 1);
}*/
/* I2C structure
uint32_t Timing
uint32_t OwnAddress1
uint32_t AddressingMode
uint32_t DualAddressMode
uint32_t OwnAddress2
uint32_t OwnAddress2Masks
uint32_t GeneralCallMode
uint32_t NoStretchMode
 */

