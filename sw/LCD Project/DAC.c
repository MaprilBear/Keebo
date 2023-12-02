// contains functions to output to the DAC

#include "./inc/tm4c123gh6pm.h"
#include "./inc/Unified_Port_Init.h"

// port definitions
#define DIN   (*((volatile uint32_t *) PD3)) // PD3, RED
#define SCLK   (*((volatile uint32_t *) PD0)) // PD0, BLUE LED
#define CS   (*((volatile uint32_t *)0x40025020)) // PF3, GREEN LED
#define FS   (*((volatile uint32_t *)PD1)) // PD1, Right Button
uint8_t volatile delay;

void DelayWait10ms2(uint32_t n) {
    uint32_t volatile time;
    while (n){
        time = 2;
        while (time){
            --time;
        }
        --n;
    }
}

// initialize PF1-4 for use outputting to DAC
// PD3: DIN
// PD0: SCLK
// PD1: FS
void DAC_Init(){
	// make CS and SCLK default high
	CS = 0;
	SCLK = 1;
	
	// FS is default low
	FS = 0;
	
	// no default value for DIN(?)
}


// output val to the DAC
void DAC_Out(uint16_t val){
  GPIO_PORTD_DATA_R |= 0x2;
	delay = 0;
  GPIO_PORTD_DATA_R &= ~0x2;
  for (int i = 15; i >= 0; i--){
    GPIO_PORTD_DATA_R = (GPIO_PORTD_DATA_R & ~0x8) | (((val & (0x1 << i)) >> i) << 3);
		delay = 1;
    GPIO_PORTD_DATA_R &= ~0x1;
		delay = 0;
    GPIO_PORTD_DATA_R |= 0x1;
  }
  GPIO_PORTD_DATA_R |= 0x2;
	delay = 1;
  GPIO_PORTD_DATA_R &= ~0x2;
}

