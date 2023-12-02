#include "./inc/tm4c123gh6pm.h"
#include "./inc/Sine.h"
#include "DAC.h"
#include "Song.h"
#include "./inc/Timer0A.h"
#include "./inc/Timer1A.h"

int32_t sinIndex;


// noteArray
#define NOTEARRAYLENGTH 1
#define FREQ 0
#define DURATION 1
uint32_t noteIndex;
uint32_t noteArray [NOTEARRAYLENGTH][2]= {{C, WHOLE}};



// calls DAC_Out
// uses Sin function to determine value to be output
void Generate_DAC_Output(){
	uint8_t outSin = Sin(sinIndex);
	DAC_Out(outSin);
	sinIndex += 1;
	sinIndex %= 256;
}

// called to change notes
// will update Timer0A with new frequency
// will update Timer1A with new duration
void Change_Notes(){
	// stop Timer0A and 1A
	Timer0A_Stop();
	Timer1A_Stop();
	
	// reinitialize timers
	noteIndex += 1;
	noteIndex %= NOTEARRAYLENGTH;
	Timer0A_Start(noteArray[noteIndex][FREQ]);
	Timer1A_Start(noteArray[noteIndex][DURATION]);
}


void Song_Init(){
	noteIndex = 0;
	sinIndex = 0;
	Timer0A_Init(&Generate_DAC_Output, noteArray[noteIndex], FREQ, 3); // interrupts to output to the DAC, frequency will change between notes
	Timer1A_Init(&Change_Notes, noteArray[noteIndex], DURATION, 4);	// timer to change notes
}

