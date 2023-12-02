#include "./inc/tm4c123gh6pm.h"


void (*FTask)(void);   // user function


void Switch_Init(void(*portFTask)(void)){
	SYSCTL_RCGCGPIO_R     |= 0x00000020;      // activate clock for Port F
  while((SYSCTL_PRGPIO_R & 0x20)==0){};     // allow time for clock to stabilize
    
  GPIO_PORTF_LOCK_R     = 0x4C4F434B;       // unlock GPIO Port F
  GPIO_PORTF_CR_R       = 0x1F;             // allow changes to PF4-0
  
  GPIO_PORTF_AMSEL_R    = 0x00;             // disable analog on PF
  GPIO_PORTF_PCTL_R     = 0x00000000;       // PCTL GPIO on PF4-0
	GPIO_PORTF_DIR_R      = 0x0E;            // PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R    = 0x00;             // disable alt funct on PF7-0
	GPIO_PORTF_PUR_R      = 0x11;             // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R      = 0x1F;             // enable digital I/O on PF4-0

	GPIO_PORTF_IS_R &= ~0x11;	// make PF4,PF0 edge sensitive
	GPIO_PORTF_IBE_R &= ~0x11;	// not both edges
	GPIO_PORTF_IEV_R &= ~0x11;	// falling edge triggered
	GPIO_PORTF_PUR_R |= 0x11;	// enable pull up resistors
	GPIO_PORTF_ICR_R = 0x11;	// clear flag 0,4
	GPIO_PORTF_IM_R |= 0x11;	// arm interrupt on PF4,PF0
	NVIC_PRI1_R = (NVIC_PRI7_R & ~NVIC_PRI7_INT30_M) | (0x6 << NVIC_PRI7_INT30_S);   // priority 6
	NVIC_EN0_R |= 0x1 << 30;	// enable NVIC
	FTask = portFTask;
}


// when button pressed
void GPIOPortF_Handler(void){
	GPIO_PORTF_ICR_R = 0x11;	// clear flag
	(*FTask)(); // execute user function
}