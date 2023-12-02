#include "./inc/tm4c123gh6pm.h"
#include <inc/Timer4A.h>
#include <src/Sound.h>
#include <src/Music.h>
#include <src/Switch.h>
#include <stdbool.h>
#include "./inc/Timer5A.h"


//uint32_t (*song)[][2];
song_t song;
uint32_t song_length;

bool envelope = false;
uint8_t envelope_index = 0;
bool paused = false;
uint8_t song_index;

volatile static short state = 0;

// PC4: play/pause
// PC5: rewind
// PC6: mode switch
void Switch_Handler(){
	uint32_t data = GPIO_PORTC_DATA_R;
	// play/pause pressed
	if((data & 0x10) == 0){
		if(paused){
			Sound_Start(song.song_array[state*2]);
			Timer4A_Start();
		}else{
			Sound_Stop();
			Timer4A_Stop();
		}
		
		paused = !paused;
	}
	// rewind pressed
	else if((data & 0x20) == 0){
		state = 0;
	}
	// mode switch
	else if((data &0x40) == 0){
		envelope = !envelope;
		song_index ^= 1;
		Music_Start(song_index);
		
	}
}

void Envelope_Increment(){
	envelope_index ++;
}


void Envelope_Init(){
	Timer5A_Init(&Envelope_Increment, (HALF+EIGTH)>>8, 7);
}


// Changes between notes in the current song based on note durations
static void Music_Step(){
  state = (state + 1) % song_length;
  if (song.song_array[state*2] == REST){
    Sound_Stop();
  } else {
    Sound_Start(song.song_array[state*2]);
  }
  Timer4A_Period(song.song_array[state*2+1]);
	// reset envelope
	if(envelope){
		envelope_index = 0;
	}

}


void Music_Init(){
	Switch_Init(&Switch_Handler);
  Sound_Init();
	Envelope_Init();
	Timer4A_Init(&Music_Step, 0xFFFFFFF, 6); 
}

// start the selected song and the associated timer
void Music_Start(short song_select){
  if (song_select == MEGALOVANIA){
    song.song_array = megalovania;
    song_length = MEGALOVANIA_LENGTH; 
  } else if (song_select == RICK_ROLL){
    song.song_array = rick;
    song_length = RICK_LENGTH; 
  }

  state = -1;
	Music_Step();
	song_index = song_select;
}

void Music_Stop(){
  Sound_Stop();
  Timer4A_Stop();
}