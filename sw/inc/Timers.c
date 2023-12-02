// ----------------------------------------------------------------------------------
// Timers.c
// Sourced from various timer files in ValvanoWare
// Authors: Jonathon Valvalo & Daniel Valvano
// 


#include <stdio.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

// ----------   Function Prototypes defined in Startup.S  ----------------------
// 
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  

void (*Timer0Task)(void);     // 1ms Timer for Clock
void (*Timer2Task)(void);     // User Function
void (*Timer5Task)(void);     // TM4C -> MQTT Function


// -------------   Timer0_Init       ----------------------------------------
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
//
void Timer0_Init(void(*task)(void), uint32_t period){
  long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R      |= 0x01;            // 0) activate TIMER0
  Timer0Task               = task;            // user function
  TIMER0_CTL_R             = 0x00000000;      // 1) disable TIMER0A during setup
  TIMER0_CFG_R             = 0x00000000;      // 2) configure for 32-bit mode
  TIMER0_TAMR_R            = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R           = period-1;        // 4) reload value
  TIMER0_TAPR_R            = 0x00000000;      // 5) bus clock resolution
  TIMER0_ICR_R             = 0x00000001;      // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R             = 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI4_R              = (NVIC_PRI4_R
                           &  0x00FFFFFF)
                           |  0x80000000;     // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R              = 1<<19;            // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R            = 0x00000001;       // 10) enable TIMER0A
  EndCritical(sr);

}

// ************************* Timer0A_Handler ***********************************

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;          // acknowledge timer0A timeout
  (*Timer0Task)();                            // execute user task
}


// ************************* Timer2_Init ***************************************
// Activate TIMER2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
//
void Timer2_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  Timer2Task = task;            // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R
                 &  0x00FFFFFF)
                 |  0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

// ************************* Timer2A_Handler ***********************************
//
void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
  (*Timer2Task)();                // execute user task
}



// ************************* Timer5_Init ****************************************
// Activate Timer5
// Inputs:  period in 12.5nsec
// Outputs: none
void Timer5_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R  |= 0x20;                // 0) activate timer5
  Timer5Task            = task;               // Point to user function    
  TIMER5_CTL_R         = 0x00000000;          // 1) disable timer5A during setup
  TIMER5_CFG_R         = 0x00000000;          // 2) configure for 32-bit timer mode
  TIMER5_TAMR_R        = 0x00000002;          // 3) configure for periodic mode, default down-count settings
  TIMER5_TAILR_R       = period-1;            // 4) reload value
  TIMER5_TAPR_R        = 0x00000000;          // 5) 12.5ns timer5A
  TIMER5_ICR_R         = 0x00000001;          // 6) clear timer5A timeout flag
  TIMER5_IMR_R        |= 0x00000001;          // 7) arm timeout interrupt
  NVIC_PRI23_R         = (NVIC_PRI23_R
                        & 0xFFFFFF00)
                        | 0x000000E0;         // 8) priority 4
  
  NVIC_EN2_R           = 0x10000000;          // 9) enable interrupt 92 in NVIC
                                              //    vector number 108, interrupt number 92
  TIMER5_CTL_R        |= 0x00000001;          // 10) enable timer5A

}

// ************************* Timer5A_Handler ***********************************
//
void Timer5A_Handler(void){ 
  TIMER5_ICR_R = TIMER_ICR_TATOCINT;          // acknowledge timer5A timeout
  (*Timer5Task)();
}  



// ************************ DelayWait10ms **************************************
// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: 
//
void DelayWait10ms(uint32_t n){
	
	uint32_t volatile time;
  while(n){
    time = 727240*2/180;  // ?????
    while(time){
	  	time--;
    }
    n--;
  }
}


