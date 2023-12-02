#ifndef __SWITCH_MATRIX__
#define __SWITCH_MATRIX__

#include "usblib/usbhid.h"
#include <stdint.h>

#define WINDOWS_KEY 0x65
#define LEFT_SHIFT 0x66
#define RIGHT_SHIFT 0x67
#define LEFT_CTRL 0x68
#define RIGHT_CTRL 0x69
#define LEFT_GUI 0x6A
#define RIGHT_GUI 0x6B
#define LEFT_ALT 0x6C
#define RIGHT_ALT 0x6D
#define CAPS_LOCK 0x6E
#define BLUETOOTH 0x6F
#define PAUSE 0xE8
#define DELETE 0x4C
#define HOME 0x4A
#define END 0x4D
#define INSERT 0x49
#define MUTE 0xEF
#define PREV 0xEA
#define NEXT 0xEB
#define CALC 0xFB
#define VOL_UP 0xED
#define VOL_DN 0xEE
#define APP_A 0xEF
#define APP_B 0xF0

void Switch_Init(void);
void Switch_Stop(void);
void Switch_Start(void);

extern uint8_t KeyboardMatrix[2][5][14]; 

#endif