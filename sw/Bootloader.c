#include "./inc/Timer2A.h"
#include "Helpers.h"
#include "inc/CortexM.h"
#include "inc/tm4c123gh6pm.h"
#include "Switch_Matrix.h"

// Apps
#include "Keyboard.h"
#include "App_BongoCat.h"
#include "App_Clock.h"

enum Apps{
    KEYBOARD, BONGO_CAT, CLOCK, NONE
} typedef Apps;

#define MAX_APPS 8

//***************************************************************************
//
//                             App loading logic
//
//***************************************************************************

uint8_t loadedApp = 0;

uint8_t tickArraySize = 0;
void(*tickArray[8])(void);

uint8_t pressArraySize = 0;
void(*pressArray[8])(uint8_t);

int8_t releaseArraySize = 0;
void(*releaseArray[8])(uint8_t);

void LoadApp(Apps app){
    switch (app){
        case KEYBOARD:
            App_Keyboard_Load();
            tickArraySize = ArrayAdd(&App_Keyboard_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayAddUint8(&App_Keyboard_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayAddUint8(&App_Keyboard_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
        case BONGO_CAT:
            App_BongoCat_Load();
            tickArraySize = ArrayAdd(&App_BongoCat_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayAddUint8(&App_BongoCat_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayAddUint8(&App_BongoCat_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
        case CLOCK:
            App_Clock_Load();
            tickArraySize = ArrayAdd(&App_Clock_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayAddUint8(&App_Clock_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayAddUint8(&App_Clock_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
    }
}

void UnloadApp(Apps app){
    switch (app){
        case KEYBOARD:
            App_Keyboard_Unload();
            tickArraySize = ArrayRemove(&App_Keyboard_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayRemoveUint8(&App_Keyboard_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayRemoveUint8(&App_Keyboard_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
        case BONGO_CAT:
            App_BongoCat_Unload();
            tickArraySize = ArrayRemove(&App_BongoCat_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayRemoveUint8(&App_BongoCat_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayRemoveUint8(&App_BongoCat_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
        case CLOCK:
            App_Clock_Unload();
            tickArraySize = ArrayRemove(&App_Clock_Tick, tickArray, tickArraySize, MAX_APPS);
            pressArraySize = ArrayRemoveUint8(&App_Clock_KeyPress, pressArray, pressArraySize, MAX_APPS);
            releaseArraySize = ArrayRemoveUint8(&App_Clock_KeyRelease, releaseArray, releaseArraySize, MAX_APPS);
            break;
    }
}

//***************************************************************************
//
//                              Event Handlers!
//
//***************************************************************************




// Main logic for each app
void AppTick(){
    for (int i = 0; i < tickArraySize; i++){
        (tickArray[i])();
    }
}

void App_KeyPress(uint8_t usageCode){
    if (usageCode == APP_A || usageCode == APP_B){
        if (loadedApp == BONGO_CAT){
            UnloadApp(BONGO_CAT);
            LoadApp(CLOCK);
            loadedApp = CLOCK;
        } else if (loadedApp == CLOCK){
            UnloadApp(CLOCK);
            loadedApp = NONE;
        } else if (loadedApp == NONE){
            LoadApp(BONGO_CAT);
            loadedApp = BONGO_CAT;
        }
        return;
    }

    for (int i = 0; i < pressArraySize; i++) {
        (pressArray[i])(usageCode);
    }
}

void App_KeyRelease(uint8_t usageCode){
    for (int i = 0; i < releaseArraySize; i++) {
        (releaseArray[i])(usageCode);
    }
}

void Bootloader_Entry(){
    Timer2A_Init(&AppTick, 2666666, 7); // Tick apps at 30 tps
    
    LoadApp(BONGO_CAT);
    LoadApp(KEYBOARD);
    
    loadedApp = BONGO_CAT;

    while(1){
        WaitForInterrupt();
    }
}

