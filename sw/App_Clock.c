#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "./inc/tm4c123gh6pm.h"
#include "./inc/Timer2A.h"
#include "./inc/ST7735.h"
#include "./inc/Timer1A.h"
#include "./inc/CortexM.h"

#define CLOCK_PERIOD 0x4C4B400 // second calibration
//#define CLOCK_PERIOD 0xCB735	// for testing
#define REFRESH_PERIOD 0xf42400

#define BACKGROUND_COLOR ST7735_BLACK
#define CLOCK_TEXT_COLOR 0xDEAD
#define ANALOG_FACE_OUTLINE_COLOR 0xBEEF
#define ALARM_TEXT_COLOR ST7735_RED
#define DIGITAL_CLOCK_FONT_SIZE 2
#define DIGITAL_CLOCK_X_OFFSET 4
#define DIGITAL_CLOCK_Y_OFFSET 0

int16_t sin_table[60] = {0,104,208,309,407,500,587,669,743,809,866,913,951,978,994,1000,994,978,951,913,866,809,743,669,587,500,406,309,207,104,0,-104,-208,-309,-407,-500,-587,-669,-743,-809,-866,-913,-951,-978,-994,-1000,-994,-978,-951,-913,-866,-809,-743,-669,-587,-500,-406,-309,-207,-104};
const int32_t CircleXbuf[180] = { 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999 };
const int32_t CircleYbuf[180] = { 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70 };

#define HOUR 0
#define MINUTE 1
#define SECOND 2


short lastTimeArray[3]; 		// {hour, minute, second}
short timeArray[3]; 			// {hour, minute, second}
short alarmArray[3] = {0,0,0}; 	// {hour, minute, second}
short lastAlarmArray[3] = {0,0,0}; 	// {hour, minute, second}
uint16_t touchDeadline[3];		// {hour, minute, second}
bool alarmEnabled = false;
bool alarmEdit = false;

#define ALARM 3

char *modeArray[4] = {"hour     ", "minute   ", "second   "};
uint16_t modeIndex = 0;

#define DIGITAL_24_HOUR 0
#define DIGITAL_12_HOUR 1
#define ANALOG			2
uint16_t dispIndex = DIGITAL_24_HOUR;
short alarmCounter = 0;
bool alarmOn = false;
bool alarmOnFirst = false;

bool justSwitchedToAlarm = false;
bool justSwitchedOffAlarm = false;

void Touch_Timestamp(void){
	// increment 15 seconds
	touchDeadline[2] = timeArray[2] + 15;
	// add in overflow to minutes and seconds
	touchDeadline[1] = timeArray[1] + touchDeadline[2] / 60;
	touchDeadline[0] = timeArray[0] + touchDeadline[1] / 60;
	
	// remove overslow
	touchDeadline[2] %= 60;
	touchDeadline[1] %= 60;
	touchDeadline[0] %= 24;
	
}

// draws analog clock face
void Draw_Analog_Face(int16_t num_color, int16_t outline_color){
	// draw numbers
	ST7735_DrawCharS(83, 58, '1', num_color, BACKGROUND_COLOR, 1);	// 1
	ST7735_DrawCharS(99, 73, '2', num_color, BACKGROUND_COLOR, 1);	// 2
	ST7735_DrawCharS(105, 93, '3', num_color, BACKGROUND_COLOR, 1);	// 2
	ST7735_DrawCharS(99, 114, '4', num_color, BACKGROUND_COLOR, 1);	// 4
	ST7735_DrawCharS(83, 129, '5', num_color, BACKGROUND_COLOR, 1);	// 5
	ST7735_DrawCharS(61, 135, '6', num_color, BACKGROUND_COLOR, 1);	// 6
	ST7735_DrawCharS(39, 129, '7', num_color, BACKGROUND_COLOR, 1);	// 7
	ST7735_DrawCharS(23, 114, '8', num_color, BACKGROUND_COLOR, 1);	// 8
	ST7735_DrawCharS(17, 94, '9', num_color, BACKGROUND_COLOR, 1);	// 9
	ST7735_DrawCharS(23, 73, '1', num_color, BACKGROUND_COLOR, 1);	// 10
	ST7735_DrawCharS(29, 73, '0', num_color, BACKGROUND_COLOR, 1);
	ST7735_DrawCharS(38, 57, '1', num_color, BACKGROUND_COLOR, 1);	// 11
	ST7735_DrawCharS(44, 57, '1', num_color, BACKGROUND_COLOR, 1);
	ST7735_DrawCharS(59, 52, '1', num_color, BACKGROUND_COLOR, 1);	// 12
	ST7735_DrawCharS(65, 52, '2', num_color, BACKGROUND_COLOR, 1);

	// draw circle outline
	ST7735_XYplot(180,(int32_t *)CircleXbuf, (int32_t *)CircleYbuf, outline_color);
			
}

/*
void Clock_Button_Task(){
	// update the next time that you need to touch it (used in alarm set mode only)
	if(modeIndex == 3){
		Touch_Timestamp();
	}

	// Enable or disable the set alarm, or clear the current alarm
	if ((GPIO_PORTF_DATA_R & 0x1) == 0){
		
		if(alarmOn){
			// turn off alarm
			ST7735_InvertDisplay(0);
			alarmOn = false;
			ST7735_SetCursor(0,4);
			ST7735_OutString("               \n               ");
			ST7735_SetCursor(0,0);
			
		} else {
			//toggle alarm
			alarmEnabled = !alarmEnabled;
		}
	}

	if ((GPIO_PORTF_DATA_R & (0x1 << 4)) == 0){
		if (alarmEdit == false){
			justSwitchedToAlarm = true;
		} else {
			justSwitchedOffAlarm = true;
		}
		alarmEdit = !alarmEdit;
	}
	
	// incrementing
	if((GPIO_PORTC_DATA_R & 0x10) == 0){
		// increment hour/min/sec
		if (!alarmEdit){
			if (modeIndex == SECOND){
				timeArray[SECOND]++;

				if (timeArray[SECOND] >= 60){
					timeArray[SECOND] %= 60;
					timeArray[MINUTE]++;

					if (timeArray[MINUTE] >= 60){
						timeArray[MINUTE] %= 60;
						timeArray[HOUR]++;

						if (timeArray[HOUR] >= 24){
							timeArray[HOUR] %= 24;
						}
					}
				}
			} else if (modeIndex == MINUTE){
				timeArray[MINUTE]++;

				if (timeArray[MINUTE] >= 60){
					timeArray[MINUTE] %= 60;
					timeArray[HOUR]++;

					if (timeArray[HOUR] >= 24){
						timeArray[HOUR] %= 24;
					}
				}
			} else {
				timeArray[HOUR]++;
				if (timeArray[HOUR] >= 24){
					timeArray[HOUR] %= 24;
				}
			}
		}
		// increment alarm
		else {

			lastAlarmArray[HOUR] = alarmArray[HOUR];
			lastAlarmArray[MINUTE] = alarmArray[MINUTE];
			lastAlarmArray[SECOND] = alarmArray[SECOND];
			
			if (modeIndex == SECOND){
				alarmArray[SECOND]++;

				if (alarmArray[SECOND] >= 60){
					alarmArray[SECOND] %= 60;
					alarmArray[MINUTE]++;

					if (alarmArray[MINUTE] >= 60){
						alarmArray[MINUTE] %= 60;
						alarmArray[HOUR]++;

						if (alarmArray[HOUR] >= 24){
							alarmArray[HOUR] %= 24;
						}
					}
				}
			} else if (modeIndex == MINUTE){
				alarmArray[MINUTE]++;

				if (alarmArray[MINUTE] >= 60){
					alarmArray[MINUTE] %= 60;
					alarmArray[HOUR]++;

					if (alarmArray[HOUR] >= 24){
						alarmArray[HOUR] %= 24;
					}
				}
			} else {
				alarmArray[HOUR]++;
				if (alarmArray[HOUR] >= 24){
					alarmArray[HOUR] %= 24;
				}
			}
		}
	}
	
	// decrementing
	else if((GPIO_PORTC_DATA_R & 0x20) == 0){
		//decrements hour

		if (!alarmEdit){
			if (modeIndex == SECOND){
        timeArray[SECOND]--;
				if (timeArray[SECOND] < 0){
          timeArray[MINUTE]--;
					if (timeArray[MINUTE] < 0){
            timeArray[HOUR]--;
						if (timeArray[HOUR] < 0){
							timeArray[HOUR] += 24;
						}

						timeArray[MINUTE] += 60;
					}

					timeArray[SECOND] += 60;
				}

			} else if (modeIndex == MINUTE){
        timeArray[MINUTE]--;
				if (timeArray[MINUTE] < 0){
          timeArray[HOUR]--;
					if (timeArray[HOUR] < 0){
						timeArray[HOUR] += 24;
					}

					timeArray[MINUTE] += 60;
				}
			} else if (modeIndex == HOUR){
        timeArray[HOUR]--;
        if (timeArray[HOUR] < 0){
          timeArray[HOUR] += 24;
        }
      }
		}
		// TODO IN CLASS
		// decrement alarm if alarm enabled
		else {

			lastAlarmArray[HOUR] = alarmArray[HOUR];
			lastAlarmArray[MINUTE] = alarmArray[MINUTE];
			lastAlarmArray[SECOND] = alarmArray[SECOND];

			if (modeIndex == SECOND){
        alarmArray[SECOND]--;
				if (alarmArray[SECOND] < 0){
          alarmArray[MINUTE]--;
					if (alarmArray[MINUTE] < 0){
            alarmArray[HOUR]--;
						if (alarmArray[HOUR] < 0){
							alarmArray[HOUR] += 24;
						}

						alarmArray[MINUTE] += 60;
					}

					alarmArray[SECOND] += 60;
				}

			} else if (modeIndex == MINUTE){
        alarmArray[MINUTE]--;
				if (alarmArray[MINUTE] < 0){
          alarmArray[HOUR]--;
					if (alarmArray[HOUR] < 0){
						alarmArray[HOUR] += 24;
					}

					alarmArray[MINUTE] += 60;
				}
			} else if (modeIndex == HOUR){
        alarmArray[HOUR]--;
        if (alarmArray[HOUR] < 0){
          alarmArray[HOUR] += 24;
        }
      }
		}
	}
	// changing mode
	else if((GPIO_PORTC_DATA_R & 0x80) == 0){
		modeIndex += 1;
		modeIndex %= 3;
	}
// make PE2 analog/digital toggle
	else if((GPIO_PORTC_DATA_R & 0x40) == 0){
		
		if(dispIndex == DIGITAL_24_HOUR){	// 24 hour digital mode
			ST7735_FillRect(0, 30, 128, 130,0);
			dispIndex = DIGITAL_12_HOUR;
		} else if (dispIndex == DIGITAL_12_HOUR){	// 12 hour digital mode
			ST7735_FillRect(0, 30, 128, 130,0);
			
			if (alarmEdit){
				Draw_Analog_Face(ALARM_TEXT_COLOR, ALARM_TEXT_COLOR);
			} else {
				Draw_Analog_Face(CLOCK_TEXT_COLOR, ANALOG_FACE_OUTLINE_COLOR);
			}
			dispIndex = ANALOG;
		}
		else {	// analog mode
			ST7735_FillRect(0, 30, 128, 130,0);
			dispIndex = DIGITAL_24_HOUR;
		}
	}
}
*/

// prints alarm on third row of regular text
void Print_Alarm(short hour, short min, short sec){
	bool am = true;
	if (dispIndex == DIGITAL_12_HOUR){
		if (hour > 12){
			hour -= 12;
			am = false;
		}
		if (hour == 0){
			hour = 12;
		}
	}
	ST7735_OutUDec(hour/10);
	ST7735_OutUDec(hour%10);
	ST7735_OutChar(':');
	ST7735_OutUDec(min/10);
	ST7735_OutUDec(min%10);
	ST7735_OutChar(':');
	ST7735_OutUDec(sec/10);
	ST7735_OutUDec(sec%10);
	if(dispIndex == DIGITAL_12_HOUR || dispIndex == ANALOG){
		if(am){
			ST7735_OutString("AM");
		} else {
			ST7735_OutString("PM");
		}
	} else {
		// clear AM PM when in 24 hour or analog mode
		ST7735_OutString("  ");
	}
}



// displays digital clock
void Clock_Display_Digital(short hour, short min, short sec, int16_t color){
	if(dispIndex == 1){
		ST7735_SetCursor(9,8);
		if (hour < 12){
			ST7735_OutStringCustom("AM", color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE - 1, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
		} else {
			ST7735_OutStringCustom("PM", color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE - 1, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
			hour -= 12;
		}
		if(hour == 0){
			hour = 12;
		}
	}
	ST7735_SetCursor(1, 3);
	ST7735_OutUDecCustom(hour / 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutUDecCustom(hour % 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutStringCustom(":", color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutUDecCustom(min / 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutUDecCustom(min % 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutStringCustom(":", color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutUDecCustom(sec / 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
	ST7735_OutUDecCustom(sec % 10, color, BACKGROUND_COLOR, DIGITAL_CLOCK_FONT_SIZE, DIGITAL_CLOCK_X_OFFSET, DIGITAL_CLOCK_Y_OFFSET);
}

#define CENTER_X 64
#define CENTER_Y 96
#define HOUR_HAND_LENGTH 15
#define MINUTE_HAND_LENGTH 25
#define SECOND_HAND_LENGTH 35

void DrawHand(short hand_length, short input, uint16_t color){
	uint16_t x = (hand_length * sin_table[60 - input]) / 1000;
	uint16_t y = (hand_length * sin_table[60 - (input + 15) % 60]) / 1000;
	ST7735_Line(CENTER_X, CENTER_Y, CENTER_X - x, CENTER_Y + y, color);
}

// displays analog clock
void Clock_Display_Analog(short hour, short minute, short second){
	//erase current hands
	//ST7735_FillRect(39, 70, 50, 50, 0);

	// I have no idea why the below doesn't fire when it should, so for now it's if true
	//if (lastTimeArray[HOUR] != hour){
	if (true) {
		// Clear old hour hand
		uint16_t total_min_display = (60 * (lastTimeArray[HOUR] % 12)  + lastTimeArray[MINUTE]) / 12;
		uint16_t hour_x = (HOUR_HAND_LENGTH * sin_table[total_min_display]) / 1000;
		uint16_t hour_y = (HOUR_HAND_LENGTH * sin_table[(total_min_display + 15) % 60]) / 1000;
		ST7735_Line(CENTER_X, CENTER_Y, CENTER_X + hour_x, CENTER_Y - hour_y, BACKGROUND_COLOR);
	}

	if (lastTimeArray[MINUTE] != minute){
		// Clear old minute hand
		DrawHand(MINUTE_HAND_LENGTH, lastTimeArray[MINUTE], BACKGROUND_COLOR);
	}

	if (lastTimeArray[SECOND] != second){
		// Clear old second hand
		DrawHand(SECOND_HAND_LENGTH, lastTimeArray[SECOND], BACKGROUND_COLOR);
	}
	
	// draw AM/PM under hands
	if (timeArray[HOUR] < 12){
		ST7735_DrawCharS(80,93, 'A', CLOCK_TEXT_COLOR, BACKGROUND_COLOR, 1);
		ST7735_DrawCharS(86,93, 'M', CLOCK_TEXT_COLOR, BACKGROUND_COLOR, 1);
	} else {
		ST7735_DrawCharS(80,93, 'P', CLOCK_TEXT_COLOR, BACKGROUND_COLOR, 1);
		ST7735_DrawCharS(86,93, 'M', CLOCK_TEXT_COLOR, BACKGROUND_COLOR, 1);
	}

	// draw hour hands
	uint16_t total_min_display = (60 * (hour % 12)  + minute) / 12;
	uint16_t hour_x = (HOUR_HAND_LENGTH * sin_table[total_min_display]) / 1000;
	uint16_t hour_y = (HOUR_HAND_LENGTH * sin_table[(total_min_display + 15) % 60]) / 1000;
	ST7735_Line(CENTER_X, CENTER_Y, CENTER_X + hour_x, CENTER_Y - hour_y, ST7735_WHITE);
	
	// minute hand
	DrawHand(MINUTE_HAND_LENGTH, minute, ST7735_WHITE);

	// second hand
	DrawHand(SECOND_HAND_LENGTH, second, ST7735_RED);

	
}

// Clock Refresh function, runs at 5Hz
void Clock_Refresh(){
	// check if alarm was triggered

	if (alarmOnFirst){
		ST7735_SetCursor(0,4);
		ST7735_OutString("ALARM!");
		alarmOn = true;
    alarmOnFirst = false;
	}

	if (justSwitchedToAlarm && dispIndex == ANALOG){
		Draw_Analog_Face(ALARM_TEXT_COLOR, ALARM_TEXT_COLOR);
		justSwitchedToAlarm = false;
	}

	if (justSwitchedOffAlarm && dispIndex == ANALOG){
		Draw_Analog_Face(CLOCK_TEXT_COLOR, ANALOG_FACE_OUTLINE_COLOR);
		justSwitchedOffAlarm = false;
	}

	// grab time before we can be interrupted
	short tempHour = alarmEdit ? alarmArray[HOUR] : timeArray[HOUR];
	short tempMinute = alarmEdit ? alarmArray[MINUTE] : timeArray[MINUTE];
	short tempSecond = alarmEdit ? alarmArray[SECOND] : timeArray[SECOND];
	
	// make display invert colors if alarm is on
	if(alarmOn){
		ST7735_InvertDisplay(alarmCounter & 0x2);
		alarmCounter += 1;
	}
	
	// display current mode settings
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Edit mode: ");
	ST7735_OutString(modeArray[modeIndex]);
	ST7735_SetCursor(0, 1);
	ST7735_OutString("Alarm at ");
	Print_Alarm(alarmArray[HOUR], alarmArray[MINUTE], alarmArray[SECOND]);
	ST7735_SetCursor(0,2);
	if (alarmEnabled){
		ST7735_OutString("Alarm is ENABLED ");
	} else {
		ST7735_OutString("Alarm is DISABLED");
	}
	
	if(dispIndex == 2){
		Clock_Display_Analog(tempHour, tempMinute, tempSecond);
	} else {
		Clock_Display_Digital(tempHour, tempMinute, tempSecond, alarmEdit ? ALARM_TEXT_COLOR : CLOCK_TEXT_COLOR);
	}

	// update last seen times
	lastTimeArray[HOUR] = tempHour;
	lastTimeArray[MINUTE] = tempMinute;
	lastTimeArray[SECOND] = tempSecond;
}

// Runs every second from Timer1A's ISR
void Clock_ISR(){

	if (++timeArray[SECOND] >= 60){
		timeArray[SECOND] %= 60;
		if (++timeArray[MINUTE] >= 60){
			timeArray[MINUTE] %= 60;
			if (++timeArray[HOUR] >= 24){
				timeArray[HOUR] %= 24;
			}
		}
	}
	
	// If we hit the alarm time, tigger the alarm!
	if (alarmEnabled && timeArray[0] == alarmArray[0] && timeArray[1] == alarmArray[1] && timeArray[2] == alarmArray[2]){
		alarmOnFirst = true;
	}

	// if in alarm set mode and hasn't been touched in 15 seconds, go back to hour edit mode
	if(modeIndex == 3 && timeArray[0] == touchDeadline[0] && timeArray[1] == touchDeadline[1] && timeArray[2] == touchDeadline[2]){
		alarmEdit = false;
		justSwitchedOffAlarm = true;
	}
    
    Clock_Refresh();
}

void App_Clock_Load(void) {

    /* Interrupts currently being used:
        Timer2A>
	
    /* Start up display. */
    ST7735_InitR(INITR_REDTAB);
		
    /* Reset screen. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetRotation(2);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Starting...\n");
	
    // Start our clock!
    //Output_Clear();
	timeArray[HOUR] = 0;
	timeArray[MINUTE] = 0;
	timeArray[SECOND] = 0;

	lastTimeArray[HOUR] = 0;
	lastTimeArray[MINUTE] = 0;
	lastTimeArray[SECOND] = 0;

    DisableInterrupts();
	// Register our Time ISR and Graphics ISR with Timer1A and B and start it
	//Timer2A_Init(&Clock_ISR, CLOCK_PERIOD, 5);

	// Register our Graphics ISR with Timer1B and start it, this will trigger 5 times per second (hopefully)
	//Timer1A_Init(&Clock_Refresh, REFRESH_PERIOD, 3);

    EnableInterrupts();

    ST7735_ClockPlotInit(-2500, 2500, -2500, 2500);

    Clock_Refresh();

}

void App_Clock_KeyPress(uint8_t usageCode){
    return;
}

void App_Clock_KeyRelease(uint8_t usageCode){
    return;
}

void App_Clock_Tick(){
    static int count = 0;
    if (count++ % 30 == 0){
        Clock_ISR();
    }
}

void App_Clock_Unload(){
    //Timer2A_Stop();
    //Timer1A_Stop();
    ST7735_FillScreen(ST7735_BLACK);
}