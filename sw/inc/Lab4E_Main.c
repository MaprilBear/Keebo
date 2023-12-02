// -------------------------------------------------------------------
// File name:     Lab4E_Main.c
// Description:   This code is the main loop for the new MQTT Clock Control IOT Lab
//               
// Authors:       Mark McDermott
// Date:          June 6, 2023
//
// 

#include <stdio.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#include "ST7735.h"
#include "PLL.h"
#include "Timers.h"
#include "UART2.h"
#include "UART5.h"
#include "esp8266.h"
#include "MQTT.h"
//#include "Lab4E.h"
#include "ADC.h"
#include "Clock.h"


//----- Prototypes of functions in startup.s  ----------------------
//
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // Go into low power mode

// -----------------------------------------------------------------
// -------------------- MAIN LOOP ----------------------------------
//
int main(void){       
  PLL_Init();                 // Bus clock at 80 MHz
  DisableInterrupts();        // Disable interrupts until finished with inits
  Port_Init();								// Initialize the Ports used for this lab
  Output_Init();              // Initialize ST7735 LCD
  UART2_Init();               // Enable Debug Serial Port
  UART5_Init();               // Enable ESP8266 Serial Port
  Reset_8266();               // Reset the WiFi module
  SetupWiFi();                // Setup communications to MQTT Broker via 8266 WiFi
  Init_Clock();								// Setup the Clock
  
  Timer0_Init(&clock_1ms,80000);              // Timer0 trigger every 1ms 
  Timer2_Init(&MQTT_to_TM4C, 400000);         // Get data every 5ms 
  Timer5_Init(&TM4C_to_MQTT, 80000000);       // Send data back to MQTT Web App every second 
  
  //ADC0_InitTimer3ATriggerSeq3(33000000);    // Initialize the ADC that measures temperature.
  
  EnableInterrupts();

  while(1){   
    Run_Clock();              // Run the clock
    WaitForInterrupt();       // Wait to run the clock until the next interrupt
  }
}

