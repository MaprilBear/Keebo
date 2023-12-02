// -------------------------------------------------
// UART1.c
// Runs on TM4C123
// Simple device driver for the UART1 which is connected to ESP8266
//
// Author:      Daniel Valvano
// Date:        May 23, 2014
// Modified by: Mark McDermott
// Date:        May 25, 2018 
//
//-------------------------------------------------
// Pin configuration:
//
// U1Rx   connected to PC4
// U1Tx   connected to PC5


#include <stdio.h>
#include <stdint.h>
#include "UART1.h"

//#include "inc/tm4c123gh6pm.h"

#define UART1_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART1_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART1_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART1_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART1_CTL_UARTEN         0x00000001  // UART Enable
#define CR 0x0D
#define LR 0x0A

// 115200 baud, odd parity, one stop bit, FIFOs

void UART1_Init(void){

  // Enable UART1
  SYSCTL_RCGCUART_R               |= SYSCTL_RCGCUART_R1;
  while((SYSCTL_PRUART_R & SYSCTL_PRUART_R1) == 0){};                 // Wait

  // Enable PORT C clock gating
  
  SYSCTL_RCGCGPIO_R               |= SYSCTL_RCGCGPIO_R2;
  while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2) == 0){};                // allow time to finish activating

  GPIO_PORTC_AFSEL_R              |=  (1 << 4) + (1 << 5);               // Enable alt function for PC4 and PC5
    
  GPIO_PORTC_PCTL_R                &= ~(GPIO_PCTL_PC4_M | GPIO_PCTL_PC5_M); // clear PC4 and PC5 PCTL muxes
  GPIO_PORTC_PCTL_R				  |= GPIO_PCTL_PC4_U1RX | GPIO_PCTL_PC5_U1TX; // set muxes to UART1

  GPIO_PORTC_DIR_R                |=  (1 << 5);           // Set PC5 to OUTPUT
  GPIO_PORTC_DEN_R                |=  (1 << 4) + (1 << 5);    // Enable digital
  UART1_CTL_R                     &= ~UART_CTL_UARTEN;    // Clear UART1 enable bit during config
	
  UART1_IBRD_R 					   = 43;                    // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
  UART1_FBRD_R 					   = 26;                    // FBRD = round(0.4028 * 64 ) = 26
                                        
  UART1_LCRH_R 					   = (UART_LCRH_WLEN_8|UART_LCRH_FEN|UART_LCRH_PEN); // 8 bit word length (odd parity, one stop bit, FIFOs)
  UART1_IFLS_R 					  &= ~0x3F;                 // Clear TX and RX interrupt FIFO level fields
  UART1_CTL_R 					  |= UART_CTL_UARTEN;       // enable UART
}


//------------   UART1_InChar   ------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
//
char UART1_InChar(void){
  while((UART1_FR_R&UART1_FR_RXFE) != 0);
  return((char)(UART1_DR_R & 0xFF));
}


//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
//
int UART1_OutChar(char data){
  int count = 0;
  while(((UART1_FR_R&UART1_FR_TXFF) != 0) && (count++ < 100));
  if (count >= 90){
    return 1;
  } else {
    UART1_DR_R = data;
    return 0;
  }
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
//
void UART1_OutString(char *pt){
  while(*pt){
    UART1_OutChar(*pt);
    pt++;
  }
}

//---------------------UART1_Out_CRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
//
void UART1_Out_CRLF(void){
  UART1_OutChar(CR);
  UART1_OutChar(LF);
}


//------------UART1_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
// and converts to a 32-bit unsigned number
// valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
//
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
//

uint32_t UART1_InUDec(void){
uint32_t number=0, length=0;
char character;
  character = UART1_InChar();
  while(character != CR){             // accepts until <enter> is typed
    
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 4294967295
      length++;
      UART1_OutChar(character);
    }
    
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    
    else if((character==BS) && length){
      number /= 10;
      length--;
      UART1_OutChar(character);
    }
    character = UART1_InChar();
  }
  return number;
}

//-----------------------UART1_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
//

void UART1_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
//
  if(n >= 10){
    UART1_OutUDec(n/10);
    n = n%10;
  }
  UART1_OutChar(n+'0'); /* n is between 0 and 9 */
}


//---------------------UART1_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
//
uint32_t UART1_InUHex(void){
uint32_t number=0, digit, length=0;
char character;
  character = UART1_InChar();
  while(character != CR){
    digit = 0x10; // assume bad
    if((character>='0') && (character<='9')){
      digit = character-'0';
    }
    else if((character>='A') && (character<='F')){
      digit = (character-'A')+0xA;
    }
    else if((character>='a') && (character<='f')){
      digit = (character-'a')+0xA;
    }
// If the character is not 0-9 or A-F, it is ignored and not echoed
    if(digit <= 0xF){
      number = number*0x10+digit;
      length++;
      UART1_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if((character==BS) && length){
      number /= 0x10;
      length--;
      UART1_OutChar(character);
    }
    character = UART1_InChar();
  }
  return number;
}

//--------------------------UART1_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
//

void UART1_OutUHex(uint32_t number){

	// This function uses recursion to convert the number of
	//   unspecified length as an ASCII string
  
  if(number >= 0x10){
    UART1_OutUHex(number/0x10);
    UART1_OutUHex(number%0x10);
  }
  else{
    if(number < 0xA){
      UART1_OutChar(number+'0');
     }
    else{
      UART1_OutChar((number-0x0A)+'A');
    }
  }
}


//------------UART1_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --

void UART1_InString(char *bufPt, uint16_t max) {
int length=0;
char character;
  character = UART1_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART1_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART1_OutChar(character);
    }
    character = UART1_InChar();
  }
  *bufPt = 0;
}


