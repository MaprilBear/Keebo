#include "CPM.h"
#include <stdint.h>
/* Clock delay and interrupt control. */
#include "./inc/CortexM.h"
/* Initialization of all the pins. */
#include "./inc/Unified_Port_Init.h"
/* ST7735 display. */
#include "./inc/ST7735.h"
#include "./inc/Timer0A.h"

#define CPS_LEN 5

uint8_t chars_per_sec[CPS_LEN];
uint8_t CPS_Index;



void CPM_Init(void){
	// init variables
	for(int i = 0; i < CPS_LEN; i ++){
		chars_per_sec[i] = 0;
	}
	CPS_Index = 0;
	
	//
	ST7735_SetCursor(0,0);
	char message[] = "Char per min:";
	//ST7735_DrawStringCustom(5, 10, message, ST7735_WHITE, ST7735_BLACK, 4, 1, 1);
	 ST7735_OutStringCustom(message, ST7735_WHITE, ST7735_BLACK, 1, 5, 35);
	
	// start 1 sec timer
	Timer0A_Init(&CPM_Timer_Handler, 0x4C4B400, 3);
	
	CPM_Disp(0);
}


void CPM_Stop(void){
	// stop timer
	Timer0A_Stop();
	
	// clear screen
	ST7735_PlotClear(20,130);
}

// output
void CPM_Disp(uint16_t CPM){
	ST7735_SetCursor(0,0);
	ST7735_OutCPM(CPM, 7, 5, 60);
}


// called every second
void CPM_Timer_Handler(void){
	// average CPM of the last 5 seconds
	uint16_t CPM = 0;
	for(int i = 0; i < CPS_LEN; i ++){
		CPM += chars_per_sec[i];
	}
	CPM *= 60/CPS_LEN;
	
	// move index, and zero oldest data
	// new data will overwrite
	CPS_Index += 1;
	CPS_Index %= CPS_LEN;
	chars_per_sec[CPS_Index] = 0;	
	
	CPM_Disp(CPM);
}


// increment number of keypresses per second
void CPM_Keypress(void){
	chars_per_sec[CPS_Index] += 1;
}