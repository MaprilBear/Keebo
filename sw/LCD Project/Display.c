#include "Display.h"
#include "Bongo.h"
#include "CPM.h"
#include "Pet.h"
#include "./inc/tm4c123gh6pm.h"
#include <stdint.h>

#define CPM 0
#define BONGO 1
#define PET 2
#define NUM_OPTIONS 2

uint8_t mode = 0;

void Switch_Handler(){
	uint32_t state = GPIO_PORTF_DATA_R & 0x11;
	
	if(state == 1) // keypress
		{
		if(mode == CPM){
			CPM_Keypress();
		}else if(mode == BONGO){
			Bongo_Keypress();
		}//else if(mode == PET){
//			Pet_Keypress();
//		}
	}	
	else // mode
	{
		// stop old mode
		if(mode == CPM){
			CPM_Stop();
		}else if(mode == BONGO){
			Bongo_Stop();
		}//else if(mode == PET){
//			Pet_Stop();
//		}
		mode += 1;
		mode %= NUM_OPTIONS;
		// start new mode
		if(mode == CPM){
			CPM_Init();
		}else if(mode == BONGO){
			Bongo_Init();
		}//else if(mode == PET){
//			Pet_Init();
//		}
	}
}