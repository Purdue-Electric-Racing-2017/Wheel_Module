/***************************************************************************
*
*     File Information
*
*     Name of File:Wheel_Speed_Sensor.c
*
*     Authors (Include Email):
*       1. Matt Gaydos
*       2. Matt Flanagan
*
*     File dependents: (header files, flow charts, referenced documentation)
*       1.josh's can thing
*       2.Wheel_Speed_Sensor.h
*
*     File Description: (Describe overall what part of the car the file
*       addresses as well as any communication protocols it uses.
*
***************************************************************************/

#define RADS_BETWEEN 0.28559933214452665804205848938905 //it is the distance of radians between the metal nodes of which there are 22 in 2pi radians simple math gives you that number

/***************************************************************************
*     Function Information
*     Name of Function: init_wheel_speed
*
*     Programmer's Name: Matt Gaydos
*						 Matt Flanagan
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.none
*
*      Global Dependents:
*	   1. none
*     Function Description: This will initialize speed to zero
***************************************************************************/
int init_wheel_speed() {
	return 0;
}


int wheel_main() {
	//initializations
	//get initial time

	//while loop that will run for .1 seconds .1 = final_time - init_time
		//read the hal pin need timeout period
		//if the hal pin is high add to a counter

	//at this point we have time = .1 seconds and count
	//use these to calculate speed
	//write this to a register in the MC using HAL

	//call can function
	//return appropriate error or no error
}


/***************************************************************************
*     Function Information
*     Name of Function: read_hal_pin
*
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: bool if it is high or low
*
*     Parameters (list data type, name, and comment one per line):
*       1.none
*
*      Global Dependents:
*	   1.none
*     Function Description: It is a function that will be used to determine the state of the pin that the wheel speed module outputs too
***************************************************************************/
bool read_hal_pin() {
	//read hal pin value
	//return it
}

/***************************************************************************
*     Function Information
*     Name of Function: get_diff_time
*
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: double delta_t
*
*     Parameters (list data type, name, and comment one per line):
*       1.double final time
*       2. doube begin time
*
*      Global Dependents:
*	   1.none
*     Function Description: gets the change in time used for the while loop condition and calculating speed
***************************************************************************/
double get_diff_time(double final, double begin) {
	return final - begin;
}

/***************************************************************************
*     Function Information
*     Name of Function: calc_speed
*
*     Programmer's Name: Matt Gaydos
*     					 Matt Flangan
*
*     Function Return Type: double speed in rad/sec
*
*     Parameters (list data type, name, and comment one per line):
*       1. int count how many times a node has been passed
*       2. double delta_t how much time has gone by
*
*      Global Dependents:
*	   1.None
*     Function Description: given the amount of nodes that have passed by in time
*     given that you can figure out distance travelled in radians and then can get
*     radians per second
***************************************************************************/
double calc_speed(int count, double delta_t) {
	return count * RADS_BETWEEN / delta_t;	//units are in rads per second
}

/***************************************************************************
*     Function Information
*     Name of Function: write_speed_to_register
*
*     Programmer's Name: Matt Flanagan
*     					 Matt Gaydos
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. double speed in radians per second
*		2. void * ptr a pointer to the register the speed needs to be stored to
*      Global Dependents:
*	   1. none
*     Function Description:
***************************************************************************/
void write_speed_to_register(double speed, void * ptr) {
	//might be a hal to write
	double * temp_ptr = (double *) ptr;
	*temp_ptr = speed;
}






/***************************************************************************
*     Function Information
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
*     Function Description:
***************************************************************************/


