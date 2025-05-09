#include "interface.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#define special_output_port (*((volatile char*)0x20))
static int output_char(char c, FILE* stream)
{
    special_output_port = c;
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(output_char, NULL, _FDEV_SETUP_WRITE);

void        interface_init()
{
    // Set timer 1 to count cycles
    TCCR1B = 1;

    // Set output to simulator
    stdout = &mystdout;
    stderr = &mystdout;
}

void start_timing()
{
    TCNT1 = 0;
}

uint16_t end_timing()
{
    return TCNT1 - 9;
}
