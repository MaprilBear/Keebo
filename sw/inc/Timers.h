// Timers.h
// Runs on LM4F120/TM4C123
// Use Timer2 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// June 5, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
  Program 7.5, example 7.6

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

#include "inc/tm4c123gh6pm.h"


#ifndef __TIMER2INTS_H__ // do not include more than once
#define __TIMER2INTS_H__

// ***************** Timer2_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
//
void Timer2_Init(void(*task)(void), uint32_t period);

#endif // __TIMER2INTS_H__


// ***************** Timer0_Init ****************
// Activate Timer0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
//
void Timer0_Init(void(*task)(void), uint32_t period);




// ***************** Timer5_Init ****************
// Activate Timer5 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer5_Init(void(*task)(void), uint32_t period);




// ------------------------------ DelayWait10ms ---------------------------
// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: 
//
void DelayWait10ms(uint32_t n);



