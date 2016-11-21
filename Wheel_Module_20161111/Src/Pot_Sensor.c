
/***************************************************************************
*
*     File Information
*
*     Name of File: Pot_Sensor.c
*
*     Authors (Include Email):
*       1. Matt Gaydos gaydos@purdue.edu
*       2. Matt Flanagan mflanag@purdue.edu
*
*     File dependents: (header files, flow charts, referenced documentation)
*       1.Pot_Sensor.h
*       2.Josh's can thingy
*
*     File Description: This is going to be used to calculate the position of the
*     a-arm suspension. This will be done by measuring the voltage between the min and
*     max voltage
*
***************************************************************************/
#define V_MAX 5 //arbitrary values
#define V_MIN 0
#define MAX_POS 10
#define MIN_POS 2


//need to configure this as an ADC
/***************************************************************************
*     Function Information
*     Name of Function: config_ADC
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. none
*
*      Global Dependents:
*	   1.none
*     Function Description: need to configure this as an ADC
***************************************************************************/
void config_ADC() {
	//probably a hal driver
}


/***************************************************************************
*     Function Information
*     Name of Function: pot_main
*     Programmer's Name: Matt Gaydos
*     					 Matt Flanagan
*
*     Function Return Type: int the error message or not
*
*     Parameters (list data type, name, and comment one per line):
*       1.none
*
*      Global Dependents:
*	   1.none
*     Function Description: this is the main function for the potentiometer stuff.
*     In this it will get the voltage reading (in digital) and then will call calculate
*     pos then will write thsi to a register in the MC and then will finally send it
*     to the main module via can.
***************************************************************************/
int pot_main() {
	//get pin value
	//this will involve an adc hal driver

	//call calculate position

	//write the postition to a register

	//call joshs can
}

/***************************************************************************
*     Function Information
*     Name of Function: calc_pos
*     Programmer's Name: Matt Gaydos
*     					 Matt Flanagan
*     Function Return Type: double distance in unit distance
*
*     Parameters (list data type, name, and comment one per line):
*       1. double voltage the voltage that was read from the adc pin
*
*      Global Dependents:
*	   1.NONE
*     Function Description: Given the voltage and knowing that it is between
*     the min and max voltage you can find what percent of the way up it is
*     which in turn lets you find how far you are up in terms of position.
***************************************************************************/
double calc_pos(double voltage) {
	return ((voltage / (V_MAX - V_MIN) * (MAX_POS - MIN_POS)) + MIN_POS); //GET position in unit distance given the voltage
}

/***************************************************************************
*     Function Information
*     Name of Function: write_pos_to_register
*
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. double pos in unit distance
*		2. void * ptr a pointer to the register the speed needs to be stored to
*      Global Dependents:
*	   1. none
*     Function Description:
***************************************************************************/
void write_pos_to_register(double pos, void * ptr) {
	//might be a hal to write
	double * temp_ptr = (double *) ptr;
	*temp_ptr = pos;
}







/***************************************************************************
*     Function Information
*     Name of Function:
*     Programmer's Name:
*
*     Function Return Type:
*
*     Parameters (list data type, name, and comment one per line):
*       1.
*
*      Global Dependents:
*	   1.
*     Function Description:
***************************************************************************/


