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

const char table_7seg[] = {
  0b01111110/*0*/,
  0b00000110/*1*/,
  0b11011010/*2*/,
  0b11001110/*3*/,
  0b10100110/*4*/,
  0b11101100/*5*/,
  0b11111100/*6*/,
  0b01000110/*7*/,
  0b11111110/*8*/,
  0b11100110/*9*/,
  0b11110110/*A*/,
  0b10111100/*B*/,
  0b01111000/*C*/,
  0b10011110/*D*/,
  0b11111000/*E*/,
  0b11110000/*F*/,
  0b00000001
};




