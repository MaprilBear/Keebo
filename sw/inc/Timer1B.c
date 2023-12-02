// Timer1B.c
// Modified by April Douglas from Timer1A.c by Daniel Valvano
// September 17th, 2023

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
  Program 7.5, example 7.6

 Copyright 2020 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void (*PeriodicTask10)(void);   // user function

// ***************** TIMER1B_Init ****************
// Activate TIMER1B interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer1B_Init(void(*task)(void), uint32_t period, uint32_t priority){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  PeriodicTask10 = task;         // user function
  TIMER1_CTL_R &= ~TIMER_CTL_TBEN;    // 1) disable TIMER1B during setup
  TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;    // 2) configure for 32-bit mode
  TIMER1_TBMR_R = TIMER_TBMR_TBMR_PERIOD;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TBILR_R = period-1;    // 4) reload value
  TIMER1_TBPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R |= TIMER_ICR_TBTOCINT;    // 6) clear TIMER1B timeout flag
  TIMER1_IMR_R |= TIMER_IMR_TBTOIM;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R & ~NVIC_PRI5_INT22_M) | (priority << NVIC_PRI5_INT22_S); // priority 
  // interrupts enabled in the main program after all devices initialized
  // vector number 38, interrupt number 22
  NVIC_EN0_R = 1<<22;           // 9) enable IRQ 22 in NVIC
  TIMER1_CTL_R |= TIMER_CTL_TBEN;    // 10) enable TIMER1A
}

void Timer1B_Handler(void){
  TIMER1_ICR_R |= TIMER_ICR_TBTOCINT;  // acknowledge TIMER1B timeout
  (*PeriodicTask10)();                 // execute user task
}
void Timer1B_Stop(void){
  NVIC_DIS0_R = 1<<22;        // 9) disable IRQ 22 in NVIC
  TIMER1_CTL_R &= ~TIMER_CTL_TBEN;  // 10) disable timer1B

}
