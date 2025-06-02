#pragma once
#include <cstring>
#include <unistd.h>
#include <ctime>

#define PROGMEM
constexpr void pinMode(...)
{
}
constexpr void attachInterrupt(...)
{
}
#define memcpy_P memcpy

#define INPUT 0
#define RISING 0

void sleep(int x)
{
    usleep(1000 * (x));
}

void delay(int x)
{
    usleep(1000 * (x));
}