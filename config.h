#ifndef CLOCK_DEFINES_H
#define CLOCK_DEFINES_H 1

#ifndef LINUX_SIM

#include <htc.h>

#define TMR0_PERIOD 0xc4

// Dip switches
#define BTN_PORT PORTB
#define BTN1 RB0
#define BTN2 RB1
#define BTN3 RB2
#define BTN4 RB3
#define BTN5 RB4
#define BTN6 RB5
#define BTN7 RB6
#define BTN8 RB7

// define putch
#define putch(X) putch_clock_display(X)
#else // NOT LINUX_SIM

typedef int bit;

char BTN_PORT;

#endif

extern const char table_7seg[];


#endif
