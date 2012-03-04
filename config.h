#ifndef CLOCK_DEFINES_H
#define CLOCK_DEFINES_H 1

#ifndef LINUX_SIM

#include <htc.h>

// timer stuff
#define TMR0_PERIOD 0xc4
#define TMR0_TRIS TRISA
#define TMR0_TRIS_MASK 0x10//input

// Dip switches
#define BTN_TRIS TRISB
#define BTN_TRIS_MASK 0xff
#define BTN_PORT PORTB
#define BTN1 RB0
#define BTN2 RB1
#define BTN3 RB2
#define BTN4 RB3
#define BTN5 RB4
#define BTN6 RB5
#define BTN7 RB6
#define BTN8 RB7

#define EDIT_BUTTON RA0
#define EDIT_BUTTON_TRIS TRISA
#define EDIT_BUTTON_TRIS_MASK 0x1

// display
#define DISPLAY_PORT PORTC
#define DISPLAY_TRIS TRISC
#define DISPLAY_TRIS_MASK 0

#define ERR 0xff

// define putch
#define putch(X) putch_clock_display(X)
#else // NOT LINUX_SIM

typedef int bit;

char BTN_PORT;
char DISPLAY_PORT;
char EDIT_BUTTON;

#endif

char button_state;
char edit_mode;

extern signed char do_command(char command);
#endif//clock_defines_h
