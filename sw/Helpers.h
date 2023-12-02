#include <stdint.h>

uint8_t ArrayAdd(void(*task)(void), void(*array[8])(void), uint8_t size, uint8_t maxSize);

uint8_t ArrayRemove(void(*task)(void), void(*array[8])(void), uint8_t size, uint8_t maxSize);

uint8_t ArrayAddUint8(void(*task)(uint8_t), void(*array[8])(uint8_t), uint8_t size, uint8_t maxSize);

uint8_t ArrayRemoveUint8(void(*task)(uint8_t), void(*array[8])(uint8_t), uint8_t size, uint8_t maxSize);