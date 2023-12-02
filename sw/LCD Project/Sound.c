// General imports.
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Device specific imports.
#include <inc/Timer3A.h>
#include <inc/Unified_Port_Init.h>
#include <inc/tm4c123gh6pm.h>
#include <DAC.h>
#include "Music.h"
#include "./inc/Dump.h"



const uint16_t sinTable[64] = {0x800,0x8c8,0x98f,0xa52,0xb0f,0xbc5,0xc71,0xd12,
0xda7,0xe2e,0xea6,0xf0d,0xf63,0xfa7,0xfd8,0xff5,
0xfff,0xff5,0xfd8,0xfa7,0xf63,0xf0d,0xea6,0xe2e,
0xda7,0xd12,0xc71,0xbc5,0xb0f,0xa52,0x98f,0x8c8,
0x800,0x737,0x670,0x5ad,0x4f0,0x43a,0x38e,0x2ed,
0x258,0x1d1,0x159,0xf2,0x9c,0x58,0x27,0xa,
0x0,0xa,0x27,0x58,0x9c,0xf2,0x159,0x1d1,
0x258,0x2ed,0x38e,0x43a,0x4f0,0x5ad,0x670,0x737};

const uint16_t envelopeTable[256] = {0,29,57,85,114,142,170,199,227,255,
	255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 
	245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 
	235, 234, 233, 232, 231, 230, 229, 228, 226, 225, 
	224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 
	214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 
	204, 203, 202, 201, 200, 198, 197, 196, 195, 194, 
	193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 
	183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 
	173, 172, 170, 169, 168, 167, 166, 165, 164, 163, 
	162, 161, 160, 159, 158, 157, 156, 155, 154, 153, 
	152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 
	141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 
	131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 
	121, 120, 119, 118, 117, 116, 115, 113, 112, 111, 
	110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 
	100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 
	89, 88, 87, 85, 84, 83, 82, 81, 80, 79, 78, 
	77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 
	66, 65, 64, 63, 62, 61, 60, 59, 58, 56, 55, 
	54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 
	43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 
	32, 31, 30, 28, 27, 26, 25, 24, 23, 22, 21, 
	20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 
	9, 8, 7, 6, 5, 4, 3, 2};
	

static void Sound_Step(void) {
  static uint8_t counter = 0;
	uint16_t out;
	if(envelope){	
		out = (envelopeTable[envelope_index] * sinTable[counter]) >> 8;
	}else{
		out = sinTable[counter];
	}
	DAC_Out(out);	
	counter++;
	counter %= 64;
}

void Sound_Init(void) {
  DAC_Init();
  Timer3A_Init(&Sound_Step, 0xFFFFFFFF, 2); 
}

void Sound_Start(short freq) {
    // 1sec / freq >> 8, ( >>8 since we need to output 256 times in a cycle for a square wave )
    Timer3A_Start((0x4C4B400 / freq) >> 7 ); 
}

void Sound_Stop(void) {
    Timer3A_Stop();
		uint32_t jit = JitterGet();
		Timer3A_Start((jit) >> 7 );
}
