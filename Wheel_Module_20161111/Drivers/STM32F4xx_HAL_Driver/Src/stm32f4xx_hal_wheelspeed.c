#include "stm32f4xx_hal.h"
//#define RADS_BETWEEN 22*0.28559933214452665804205848938905;  //check this -- value might change once discover specifics
#define radius 0.2
typedef struct{

	TIM_HandleTypeDef * htim;
	uint32_t channel;
	TIM_IC_InitTypeDef * sConfig;
	TIM2;

}wheelspeed_t;
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
	HAL_TIM_IC_Start_IT(wheelspeed_t.htim,wheelspeed_t.channel);
}
void deInitWheelSpeed(){

}
void calcSpeed(wheelspeed_t wheelspeed_t){
	while(1){
		count = __HAL_TIM_GET_COUNTER(wheelspeed_t.htim);
	}
}
void HAL_TIM_IC_CaptureCallback (TIM_HandleTypeDef * htim){
	if (ws.htim->Instance==htim->Instance){ //not sure what this line does
			input_capture_time= __HAL_TIM_GetCompare(ws.htim, ws.channel);	//read TIM2 channel 1 capture value
			__HAL_TIM_SetCounter(ws.htim, 0);	//reset counter after input capture interrupt occurs
		}
}
long getSpeedTime(){

	return input_capture_time;
}
long getSpeed(){
	return (1*radians*2*(3.1415926535)) / input_capture_time;	//units are in rads per second
}

