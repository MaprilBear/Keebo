#include "inc/tm4c123gh6pm.h"
#include "inc/Timer0A.h"
#include "inc/Unified_Port_Init.h"
#include "inc/CortexM.h"
#include "Bootloader.h"
#include "Switch_Matrix.h"
#include "Keyboard.h"
#include <stdbool.h>

#define ROW0 PF3
#define ROW1 PF2
#define ROW2 PF1
#define ROW3 PF0
#define ROW4 PC3 // TDO, must unlock


#define COL0 PC6
#define COL1 PC7
#define COL2 PD7
#define COL3 PE0
#define COL4 PE1
#define COL5 PE2
#define COL6 PE3
#define COL7 PF4
#define COL8 PB7
#define COL9 PB6
#define COL10 PE5
#define COL11 PE4
#define COL12 PB4
#define COL13 PB5

uint8_t modifier = 0;

uint16_t lastRow0 = 0;
uint16_t lastRow1 = 0;
uint16_t lastRow2 = 0;
uint16_t lastRow3 = 0;
uint16_t lastRow4 = 0;

#define DELAY 0

uint8_t KeyboardMatrix[2][5][14] = {
  {
    {HID_KEYB_USAGE_ESCAPE,  HID_KEYB_USAGE_1, HID_KEYB_USAGE_2, HID_KEYB_USAGE_3, HID_KEYB_USAGE_4, HID_KEYB_USAGE_5, HID_KEYB_USAGE_6, HID_KEYB_USAGE_7, HID_KEYB_USAGE_8, HID_KEYB_USAGE_9, HID_KEYB_USAGE_0, HID_KEYB_USAGE_MINUS, HID_KEYB_USAGE_EQUAL, HID_KEYB_USAGE_BACKSPACE},
    {HID_KEYB_USAGE_TAB, HID_KEYB_USAGE_Q, HID_KEYB_USAGE_W, HID_KEYB_USAGE_E, HID_KEYB_USAGE_R, HID_KEYB_USAGE_T, HID_KEYB_USAGE_Y, HID_KEYB_USAGE_U, HID_KEYB_USAGE_I, HID_KEYB_USAGE_O, HID_KEYB_USAGE_P, HID_KEYB_USAGE_LBRACKET, HID_KEYB_USAGE_RBRACKET, HID_KEYB_USAGE_BSLASH},
    {HID_KEYB_USAGE_CAPSLOCK, HID_KEYB_USAGE_A, HID_KEYB_USAGE_S, HID_KEYB_USAGE_D, HID_KEYB_USAGE_F, HID_KEYB_USAGE_G, HID_KEYB_USAGE_H, HID_KEYB_USAGE_J, HID_KEYB_USAGE_K, HID_KEYB_USAGE_L, HID_KEYB_USAGE_SEMICOLON, HID_KEYB_USAGE_FQUOTE, 0, HID_KEYB_USAGE_ENTER},
    {LEFT_SHIFT, 0, HID_KEYB_USAGE_Z, HID_KEYB_USAGE_X, HID_KEYB_USAGE_C, HID_KEYB_USAGE_V, HID_KEYB_USAGE_B, HID_KEYB_USAGE_N, HID_KEYB_USAGE_M, HID_KEYB_USAGE_COMMA, HID_KEYB_USAGE_PERIOD, HID_KEYB_USAGE_FSLASH, RIGHT_SHIFT, 0},
    {LEFT_CTRL, LEFT_GUI, LEFT_ALT, 0, 0, HID_KEYB_USAGE_SPACE, 0, 0, 0, HID_KEYB_USAGE_LEFT_ARROW, HID_KEYB_USAGE_UP_ARROW, HID_KEYB_USAGE_DOWN_ARROW, HID_KEYB_USAGE_RIGHT_ARROW, 0}
  },
  {
    {HID_KEYB_USAGE_BQUOTE, HID_KEYB_USAGE_F1, HID_KEYB_USAGE_F2, HID_KEYB_USAGE_F3, HID_KEYB_USAGE_F4, HID_KEYB_USAGE_F5, HID_KEYB_USAGE_F6, HID_KEYB_USAGE_F7, HID_KEYB_USAGE_F8, HID_KEYB_USAGE_F9, HID_KEYB_USAGE_F10, HID_KEYB_USAGE_F11, HID_KEYB_USAGE_F12, DELETE},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, PAUSE, HOME, END, INSERT},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, CALC, 0, BLUETOOTH, 0, MUTE, 0, APP_A, APP_B, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, PREV, VOL_DN, VOL_UP, NEXT}
  }
  } ;

void Switch_Handler(){
  DisableInterrupts();
	static short counter = 0;
	
	uint16_t currentRow0 = 0;
	uint16_t currentRow1 = 0;
	uint16_t currentRow2 = 0;
  uint16_t currentRow3 = 0;
  uint16_t currentRow4 = 0;
  
	
    
      // ROW0 - PF3
			
			// pulse row
			GPIO_PORTF_DATA_R |= (1 << 3);
			Clock_Delay1ms(DELAY);
			
			// sense columns
 
      currentRow0 = ((bool)COL0) + ((bool)COL1 << 1) + ((bool)COL2 << 2) + ((bool)COL3 << 3) 
                    + ((bool)COL4 << 4) + ((bool)COL5 << 5) + ((bool)COL6 << 6) + ((bool)COL7 << 7) 
                    + ((bool)COL8 << 8) + ((bool)COL9 << 9) + ((bool)COL10 << 10) + ((bool)COL11 << 11)
                    + ((bool)COL12 << 12) + ((bool)COL13 << 13);
  
      
      
      // End pulse
 
      GPIO_PORTF_DATA_R &= ~(1<<3);
      
      Clock_Delay1ms(DELAY);
      
      // ROW1 - PF1
			
			// pulse row
			GPIO_PORTF_DATA_R |= (1 << 1);
			Clock_Delay1ms(DELAY);
			
			// sense columns
      currentRow2 = ((bool)COL0) + ((bool)COL1 << 1) + ((bool)COL2 << 2) + ((bool)COL3 << 3) 
                    + ((bool)COL4 << 4) + ((bool)COL5 << 5) + ((bool)COL6 << 6) + ((bool)COL7 << 7) 
                    + ((bool)COL8 << 8) + ((bool)COL9 << 9) + ((bool)COL10 << 10) + ((bool)COL11 << 11)
                    + ((bool)COL12 << 12) + ((bool)COL13 << 13);
  
      
      
      // End pulse
		GPIO_PORTF_DATA_R &= ~(1<<1);
      Clock_Delay1ms(DELAY);

      // ROW2 - PF2
			
			// pulse row
			GPIO_PORTF_DATA_R |= (1 << 2);
			Clock_Delay1ms(DELAY);
			
			// sense columns
      currentRow1 = ((bool)COL0) + ((bool)COL1 << 1) + ((bool)COL2 << 2) + ((bool)COL3 << 3) 
                    + ((bool)COL4 << 4) + ((bool)COL5 << 5) + ((bool)COL6 << 6) + ((bool)COL7 << 7) 
                    + ((bool)COL8 << 8) + ((bool)COL9 << 9) + ((bool)COL10 << 10) + ((bool)COL11 << 11)
                    + ((bool)COL12 << 12) + ((bool)COL13 << 13);
  
     
      
      // End pulse
    
      GPIO_PORTF_DATA_R &= ~(1 << 2);
      Clock_Delay1ms(DELAY);

      // ROW3 - PF0
			
			// pulse row
			GPIO_PORTF_DATA_R |= (1 << 0);
			Clock_Delay1ms(DELAY);
			
			// sense columns
      currentRow3 = ((bool)COL0) + ((bool)COL1 << 1) + ((bool)COL2 << 2) + ((bool)COL3 << 3) 
                    + ((bool)COL4 << 4) + ((bool)COL5 << 5) + ((bool)COL6 << 6) + ((bool)COL7 << 7) 
                    + ((bool)COL8 << 8) + ((bool)COL9 << 9) + ((bool)COL10 << 10) + ((bool)COL11 << 11)
                    + ((bool)COL12 << 12) + ((bool)COL13 << 13);
  
      
      // End pulse
 
      GPIO_PORTF_DATA_R &= ~(1 << 0);
      Clock_Delay1ms(DELAY);
                    
      // ROW4 - PC3
			
			// pulse row
			GPIO_PORTC_DATA_R |= (1 << 3);
			Clock_Delay1ms(1);
			
			// sense columns
      currentRow4 = ((bool)COL0) + ((bool)COL1 << 1) + ((bool)COL2 << 2) + ((bool)COL3 << 3) 
                    + ((bool)COL4 << 4) + ((bool)COL5 << 5) + ((bool)COL6 << 6) + ((bool)COL7 << 7) 
                    + ((bool)COL8 << 8) + ((bool)COL9 << 9) + ((bool)COL10 << 10) + ((bool)COL11 << 11)
                    + ((bool)COL12 << 12) + ((bool)COL13 << 13);
  
      
      // End pulse
 
      GPIO_PORTC_DATA_R &= ~(1 << 3);
      Clock_Delay1ms(DELAY);
      
      // Check for modifier
      modifier = (currentRow3 & (1 << 13)) >> 13;
                    
      for (int i = 0; i < 14; i++){
          uint8_t current = (bool)(currentRow0 & (1 << i));
          uint8_t prev = (bool)(lastRow0 & (1 << i));
        
          if (current != prev){
            if (current){
              // Now on, key press
              App_KeyPress(KeyboardMatrix[modifier][0][i]);
            } else {
              // Now off, key released
              App_KeyRelease(KeyboardMatrix[modifier][0][i]);
            }
          }
      }

      for (int i = 0; i < 14; i++){
          uint8_t current = (bool)(currentRow1 & (1 << i));
          uint8_t prev = (bool)(lastRow1 & (1 << i));
        
          if (current != prev){
            if (current){
              // Now on, key press
              App_KeyPress(KeyboardMatrix[modifier][1][i]);
            } else {
              // Now off, key released
              App_KeyRelease(KeyboardMatrix[modifier][1][i]);
            }
          }
      }
      
      for (int i = 0; i < 14; i++){
          uint8_t current = (bool)(currentRow2 & (1 << i));
          uint8_t prev = (bool)(lastRow2 & (1 << i));
        
          if (current != prev){
            if (current){
              // Now on, key press
              App_KeyPress(KeyboardMatrix[modifier][2][i]);
            } else {
              // Now off, key released
              App_KeyRelease(KeyboardMatrix[modifier][2][i]);
            }
          }
      }
  
      for (int i = 0; i < 14; i++){
          uint8_t current = (bool)(currentRow3 & (1 << i));
          uint8_t prev = (bool)(lastRow3 & (1 << i));
        
          if (current != prev){
            if (current){
              // Now on, key press
              App_KeyPress(KeyboardMatrix[modifier][3][i]);
            } else {
              // Now off, key released
              App_KeyRelease(KeyboardMatrix[modifier][3][i]);
            }
          }
      }
      
      for (int i = 0; i < 14; i++){
          uint8_t current = (bool)(currentRow4 & (1 << i));
          uint8_t prev = (bool)(lastRow4 & (1 << i));
        
          if (current != prev){
            if (current){
              // Now on, key press
              App_KeyPress(KeyboardMatrix[modifier][4][i]);
            } else {
              // Now off, key released
              App_KeyRelease(KeyboardMatrix[modifier][4][i]);
            }
          }
      }
      
      lastRow0 = currentRow0;
      lastRow1 = currentRow1;
      lastRow2 = currentRow2;
      lastRow3 = currentRow3;
      lastRow4 = currentRow4;
      
      
      // re-enable JTAG if both shift keys are held
      if ((currentRow3 & 0x1001) == 0x1001){
        GPIO_PORTC_PDR_R |= (1 << 3);
        GPIO_PORTC_AFSEL_R |= (1 << 3);
      } else {
        GPIO_PORTC_PDR_R &= ~(1 << 3);
        GPIO_PORTC_AFSEL_R &= ~(1 << 3);
        SendKeyReport();
      }
      
  EnableInterrupts();
}

#define ROW0 PF3
#define ROW1 PF2
#define ROW2 PF1
#define ROW3 PF0
#define ROW4 PC3 // TDO, must unlock

void Switch_Init(){

	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R5; // Activate ports B, C, E, D, and F
	while ((SYSCTL_PRGPIO_R & (SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R5)) != (SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R5)){}
	
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R |= 1;
	GPIO_PORTF_DIR_R |= (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3); // Set rows to output
  GPIO_PORTF_DR8R_R |= (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3);
  GPIO_PORTF_PUR_R &= ~((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3));
  GPIO_PORTF_PDR_R &= ~((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3));
  GPIO_PORTF_DIR_R &= ~(1 << 4); // set col7 to IN
  GPIO_PORTF_PDR_R |= (1 << 4);
	GPIO_PORTF_DEN_R |= 1 + (1 << 1) + (1 << 2) + (1<<3) + (1 << 4);
	
  GPIO_PORTB_PDR_R |= (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
	GPIO_PORTB_DEN_R |= (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
  GPIO_PORTE_LOCK_R = 0x4C4F434B;
  GPIO_PORTE_CR_R |= (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5);
  GPIO_PORTE_PDR_R |= (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5);
  GPIO_PORTE_DEN_R |= (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5);
  
  GPIO_PORTC_LOCK_R = 0x4C4F434B;
  GPIO_PORTC_CR_R |= (1 << 3) + (1 << 6) + (1 << 7);
  GPIO_PORTC_DEN_R |= (1 << 3) + (1 << 6) + (1 << 7);
  GPIO_PORTC_DIR_R |= (1 << 3);
  GPIO_PORTC_DIR_R &= ~((1 << 6) + (1 << 7));
  GPIO_PORTC_DR8R_R |= (1 << 3);
  GPIO_PORTC_PUR_R &= ~(1 << 3);
  GPIO_PORTC_PDR_R &= ~(1 << 3);
  GPIO_PORTC_PDR_R |= (1 << 6) + (1 << 7);
  GPIO_PORTC_AFSEL_R &= ~(1 << 3);
  GPIO_PORTC_DATA_R &= ~(1 << 3);
  
    
  GPIO_PORTD_LOCK_R = 0x4C4F434B;
  GPIO_PORTD_CR_R |= (1 << 7);
  GPIO_PORTD_DEN_R |= (1 << 7);
  GPIO_PORTD_PDR_R |= (1 << 7);
	
  /*
	GPIO_PORTF_DATA_R = 0;
  GPIO_PORTE_DATA_R = 0;
  GPIO_PORTD_DATA_R = 0;
  GPIO_PORTC_DATA_R = 0;
  GPIO_PORTB_DATA_R = 0;
  */
  
	Timer0A_Init(&Switch_Handler, 800000, 4); // Poll at 100 Hz
}

void Switch_Stop(){
	Timer0A_Stop();
}
