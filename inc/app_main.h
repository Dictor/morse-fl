#ifndef MORSE_FL_APP_MAIN
#define MORSE_FL_APP_MAIN

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void PrintWeight(const char* title, int8_t* weight, int length, double scale); 
void AppMain(void);

#ifdef __cplusplus
}
#endif

#endif