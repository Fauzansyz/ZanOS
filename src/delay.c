#include "delay.h"

void delay(unsigned int t) {
    for (volatile unsigned int i = 0; i < t; i++);
}