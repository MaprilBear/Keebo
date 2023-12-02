#include <stdint.h>

uint8_t ArrayAdd(void(*task)(void), void(*array[8])(void), uint8_t size, uint8_t maxSize){
    if (size < maxSize){
        array[size] = task;
        return size + 1;
    }
    return size;
}

uint8_t ArrayRemove(void(*task)(void), void(*array[8])(void), uint8_t size, uint8_t maxSize){
    for (int i = 0; i < size; i++){
        if (array[i] == task){
            array[i] = 0;
            for (int j = i + 1; i < maxSize; i++)
            {
                array[j - 1] = array[j];
            }
            return size - 1;
        }
    }
    return size;
}

uint8_t ArrayAddUint8(void(*task)(uint8_t), void(*array[8])(uint8_t), uint8_t size, uint8_t maxSize){
    if (size < maxSize){
        array[size] = task;
        return size + 1;
    }
    return size;
}

uint8_t ArrayRemoveUint8(void(*task)(uint8_t), void(*array[8])(uint8_t), uint8_t size, uint8_t maxSize){
    for (int i = 0; i < size; i++){
        if (array[i] == task){
            array[i] = 0;
            for (int j = i + 1; i < maxSize; i++)
            {
                array[j - 1] = array[j];
            }
            return size - 1;
        }
    }
    return size;
}