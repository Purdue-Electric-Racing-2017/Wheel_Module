#include "math.h"
//#include "stm32f4xx_hal.h"


//#define RADS_BETWEEN 22*0.28559933214452665804205848938905;  //check this -- value might change once discover specifics
//#define radius 0.2;
typedef struct{

	TIM_HandleTypeDef * htim;
	uint32_t channel;
	TIM_IC_InitTypeDef * sConfig;
	double * recentSpeeds; //put in an array where store the last few values into the structure so that can allocate space to hold the values that are generated
	dount currentSpeed;

}wheelspeed_t;

long numGottenTimes[2];
int NGTIter =0;
double calculatedSpeed =0; 
long overFlowLimit =0; 
int numOverFlows =0;
long input_capture_time=0;
long count=0;
wheelspeed_t ws;
void initWheelSpeed(wheelspeed_t wheelspeed_t){
	ws = wheelspeed_t;
	HAL_TIM_IC_MspInit(wheelspeed_t.htim);
	__TIMx_CLK_ENABLE();
	__GPIOx_CLK_ENABLE(wheelspeed_t.htim);
	HAL_TIM_IC_Init();
	HAL_TIM_IC_ConfigChannel(wheelspeed_t.htim,wheelspeed_t.sConfig,wheelspeed_t.channel); //need to add other parameters -- check API
	HAL_TIM_IC_Start_IT(wheelspeed_t.htim,wheelspeed_t.channel); //write a function for the interrupt handler -- pull it from the space in memory
	overFlowLimit = __HAL_TIM_GET_AUTORELOAD(ws.htim); //this is the value at which the counter resets
	//read the reference manual to find how to access the data register
	//gonna read the count register and based on how configure prescaler & timing of system clock, can get the time --- assign that to the instance (into the array/data structure that you put into the struct at the top)
}ca
void deInitWheelSpeed(){
	//free(ws);
	//free(wheelspeed_t);
	
	/* have to call the deinit functions (TIM, etc.)
	can 0 all the variables, etc.
	
	*/
}
//the interrupt alters the data in the struct, and then the calcspeed method converts that data into an actual speed
//the calcSpeed method should also account for outliers and shit (take the average value over a time)
//could do the overflow handling here

double * calcSpeed(wheelspeed_t ws){  //this is the method that the user would call (or make a getSpeed method)
	double avg =0;
	int spdCtr =0;
	
	while(*recentSpeeds != NULL){
		avg += *recentSpeeds; 
		spdCtr++; 
		recentSpeeds++; 
	}
	if(calculatedSpeed != 0.0){
		recentSpeeds = &calculatedSpeed;
		avg += calculatedSpeed;
		spdCtr++;
		calculatedSpeed = avg/spdCtr; 
		return calculatedSpeed; 
	}
	else{
		return avg/spdCtr;
	} 
	
	 
	
}
void HAL_TIM_IC_CaptureCallback (TIM_HandleTypeDef * htim){
	if (ws.htim->Instance==htim->Instance){ //not sure what this line does
			input_capture_time= __HAL_TIM_GetCompare(ws.htim, ws.channel);	//read TIM2 channel 1 capture value
			count = __HAL_TIM_GET_COUNTER(ws.htim); //do this in the interrupt handler
			getSpeed(count);
			//__HAL_TIM_SetCounter(ws.htim, 0);	//reset counter after input capture interrupt occurs
			//should also look for a interrupt that has to do with the counter ove	rflowing -- accounnt for that
			//want the current reading - the previous reading, but account for the fact that it might overflow (just add the max counter value in this time)
		}
	if(LL_TIM_IsActiveFlag_UPDATE(ws.htim)){
		numOverFlows++;
		LL_TIM_ClearFlag_UPDATE(ws.htim); 
		count =0;
	}
}//come up with an algorithm to account for the fact that the counter overflows (can know by getting an interrupt or seeing when a flag is set) 

long getSpeedTime(){
	return input_capture_time;
}
void getSpeed(long count){
	if(numOverFlows > 0){
		count += overFlowLimit*numOverFlows;
		numOverFlows =0;
	}
	numGottenTimes[NGTIter] = count;
	if(NGTIter++ == 2){
		calculatedSpeed = numGottenTimes[1] - numGottenTimes[0]; //need to convert this into KMPH (figure out how to get distance)
		numGottenTimes[0] = 0.0;
		numGottenTimes[1] = 0.0;
		NGTIter =0;
	}
	
}

//#include "stm32f4xx_hal.h"


