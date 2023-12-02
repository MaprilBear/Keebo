// ----------------------------------------------------------------------------
//
// File name: MQTT.c
//
// Description: This code is used to bridge the TM4C123 board and the MQTT Web Application
//              via the ESP8266 WiFi board

// Authors:       Mark McDermott
// Orig gen date: June 3, 2023
// Last update:   July 21, 2023
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Clock.h"
#include "PLL.h"
#include "Timers.h"
#include "UART2.h"
#include "UART5.h"
#include "esp8266.h"

#define DEBUG1                // First level of Debug
#define DEBUG2                // Second level of Debug
#define DEBUG3                // Third level of Debug
//#undef  DEBUG1                // Comment out to enable Debug1
//#undef  DEBUG2                // Comment out to enable Debug2
//#undef  DEBUG3                // Comment out to enable Debug3

#define UART5_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART5_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART5_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART5_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART5_CTL_UARTEN         0x00000001  // UART Enable

// ----   Function Prototypes not defined in header files  ------
// 
void EnableInterrupts(void);    // Defined in startup.s

// ----------   VARIABLES  ----------------------------
//
extern uint32_t         Mode_Value;      //
extern uint32_t         Left_Value;      //
extern uint32_t         Right_Value;     //
extern uint32_t         Up_Value;        //
extern uint32_t         Down_Value;      //

extern uint32_t         ADCvalue;

char                    input_char;
char                    serial_buf[128];

char                    w2b_cmd[2];                   // MQTT Web App Command

static uint32_t         bufpos          = 0;
static uint32_t         parse           = 0;

extern unsigned short   Hour;
extern unsigned short   Minute;
extern unsigned short   Second;

uint32_t                HourLast        = 0;
uint32_t                MinuteLast      = 0;
uint32_t                SecondLast      = 0;
uint32_t                ADCvalueLast    = 0;


// --------------------------     W2B Parser      ------------------------------
//
// This parser decodes and executes commands from the MQTT Web Appication 
//
void Parser(void) {
  
  uint8_t cmd_num = atoi(w2b_cmd);     // Need to convert ASCII command to integer
  
  unsigned short New_Hour;
  unsigned short New_Minute;
  unsigned short New_Second;
  
  // ----  Command #1: Toggle MODE select 
  //
  if(cmd_num == 0x1)  {  
      if (Mode_Value == 0x1) Mode_Value =0x0;
      else if (Mode_Value == 0x0) Mode_Value = 0x1;
  }
  
  
}
  
// -----------------------  TM4C_to_MQTT Web App -----------------------------
// This routine publishes clock data to the
// MQTT Web Application via the MQTT Broker
// The data is sent using CSV format:
// MODE, Hour, Minute, Second, <- Need dangling comma to terminate string in ESP8266
// 0/1,  0-23,  0-59,    0-59
//
// ----------------------------------------------------------------------------
//    
//    Convert this routine to use a FIFO
//
// 
void TM4C_to_MQTT(void){
  char msp[24]  = ""; 
  
  sprintf(msp, "%d", Mode_Value);             // Need to add other commands here
   sprintf(msp + strlen(msp),",");     // Send dangling comma to terminate CSV
  
  UART5_OutString(msp);
  UART5_OutChar('\n');      // Send NL to indicate EOT

  #ifdef DEBUG1
   UART2_OutString("msp: ");
   UART2_OutString(msp);         
   UART2_Out_CRLF();  
  #endif 
   
}
 
// -------------------------   MQTT_to_TM4C  -----------------------------------
// This routine receives the command data from the MQTT Web App and parses the
// data and feeds the commands to the TM4C.
// -----------------------------------------------------------------------------
//
//    Convert this routine to use a FIFO
//
// 
void MQTT_to_TM4C(void) {
  parse = 0x0;
    
  if ((UART5_FR_R & UART5_FR_RXFE) ==0 ) {  // Check to see if a there is data in the RXD buffer
    input_char =(UART5_DR_R & 0xFF);        // Read the data from the UART
        
      if (input_char != '\n')               // If the incoming character is not a newline then process byte
      {                                     
        serial_buf[bufpos] = input_char;    // The buffer position in the array get assigned to the current read
        bufpos++;                           // Once that has happend the buffer advances,doing this over and over
                                            // again until the end of package marker is read.
        //UART2_OutChar(input_char);        // Debug only
      }
      else if (input_char == '\n')
      {
        serial_buf[bufpos] = ',';           // Add a comma for strcpy below
        bufpos++;
                
        if (bufpos  > 0) {
          strcpy(w2b_cmd, strtok(serial_buf, ","));
          parse = 0x1;                      // Parse incoming data
        }
        
        bufpos = 0;                         // Reset for next string
        
        #ifdef DEBUG1
        UART2_OutString("W2B Command = ");
        UART2_OutString(w2b_cmd);
        UART2_Out_CRLF();
        #endif
      } 
      
    else if ((UART5_FR_R & UART5_FR_RXFE) !=0 ) {     // No new data in the RXD buffer -> Exit routine
    }  
  } 
  
  if (parse == 0x1) Parser();                         // Call the parser if new Command received
  
}   // End of routine

