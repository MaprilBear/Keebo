// Timer1A.c
// Runs on LM4F120/TM4C123
// Use TIMER1 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Jan 1, 2020

// Modified Sep 17, 2023 by April Douglas
// Now doesn't interfere with Timer1B

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

void (*PeriodicTaskA)(void);   // user function
void (*PeriodicTaskB)(void);   // user function

// ***************** TIMER1A_Init ****************
// Activate TIMER1A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer1A_Init(void(*task)(void), uint32_t period, uint32_t priority){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  PeriodicTaskA = task;         // user function
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R |= 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R |= 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|(priority<<13); // priority 
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER1A
}

void Timer1AB_Init(void(*taskA)(void), uint32_t periodA, uint32_t priorityA, void(*taskB)(void), uint32_t periodB, uint32_t priorityB){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  PeriodicTaskA = taskA;         // user function
  PeriodicTaskB = taskB;        // user function
  TIMER1_CTL_R &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);    // 1) disable TIMER1A and B during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TBMR_R = 0x00000002;
  TIMER1_TAILR_R = periodA-1;    // 4) reload value
  TIMER1_TBILR_R = periodB-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_TBPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R |= TIMER_ICR_TATOCINT | TIMER_ICR_TBTOCINT;    // 6) clear TIMER1A and B timeout flag
  TIMER1_IMR_R |= TIMER_IMR_TATOIM | TIMER_IMR_TBTOIM;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&~NVIC_PRI5_INT21_M)|(priorityA<<NVIC_PRI5_INT21_S); // priority 
  NVIC_PRI5_R = (NVIC_PRI5_R&~NVIC_PRI5_INT22_M)|(priorityB<<NVIC_PRI5_INT22_S); // priority 
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R |= (1<<21) | (1<<22);           // 9) enable IRQ 21 and 22 in NVIC
  TIMER1_CTL_R |= TIMER_CTL_TAEN | TIMER_CTL_TBEN;    // 10) enable TIMER1A and B
}

void Timer1A_Handler(void){
  TIMER1_ICR_R |= TIMER_ICR_TATOCINT; // acknowledge TIMER1A timeout
  (*PeriodicTaskA)();                 // execute user task
}

void Timer1B_Handler(void){
  TIMER1_ICR_R |= TIMER_ICR_TBTOCINT; // acknowledge TIMER1B timeout
  (*PeriodicTaskB)();                 // execute user task
}

void Timer1A_Stop(void){
  //NVIC_DIS0_R = 1<<21;        // 9) disable IRQ 21 in NVIC
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN;  // 10) disable timer1A
}

void Timer1B_Stop(void){
  NVIC_DIS0_R = 1<<22;        // 9) disable IRQ 21 in NVIC
  TIMER1_CTL_R &= ~TIMER_CTL_TBEN;  // 10) disable timer1A
}

void Timer1A_Start(uint32_t period){
  TIMER1_CTL_R |= TIMER_CTL_TAEN;       // 10) enable timer1A
	TIMER1_TAILR_R = period-1;    // 4) reload value
}
