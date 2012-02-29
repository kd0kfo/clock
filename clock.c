#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "7seg.h"
#include "input.h"

#include "io.h"
#include "utils.h"
#include "picfs_error.h"
#include "scheduler.h"
#include "picos_time.h"
#include "version.h"

#include <stdbool.h>
#include <stddef.h>

__CONFIG(FOSC_EXTRC & WDTE_OFF & PWRTE_OFF & CP_OFF & BOREN_ON & LVP_OFF);

void tone_440(){}
void TIME_init()
{
  TIME_t new_time;
  new_time.hours = 12;
  new_time.minutes = 34;
  new_time.seconds = 56;
  new_time.month = 6;
  new_time.day = 29;
  new_time.year = 0;

  TIME_set(&new_time);

  T0CS = 1;// Transistion based on T0CLK pin
  T0IE = 1;// Increment on high to low
  T0SE = 0;
  T0IF = 0;
  GIE = 1;
  TMR0 = TMR0_PERIOD;

}

int getch()
{
  extern char button_state;
  char button_val_0 = BTN_PORT, button_val_1;
  char edit_button_val_0 = EDIT_BUTTON, edit_button_val_1;

  __delay_ms(10);// wait to debounce

  button_val_1 = BTN_PORT;
  edit_button_val_1 = EDIT_BUTTON;

#if 0
  button_state = (button_val_0 & button_val_1) | (button_val_0 ^ ~button_val_1);
  edit_mode = (edit_button_val_0 & edit_button_val_1) | (edit_button_val_0 ^ ~edit_button_val_1);
#else
  button_state = (button_val_0 & button_val_1);
  edit_mode = (edit_button_val_0 & edit_button_val_1);
#endif
  edit_mode &= 1;

  return (int)button_state;
  
}

void interrupt isr()
{
  if(T0IF && T0IE)
    {
      TIME_tick();
      TMR0 = TMR0_PERIOD;
      T0IF = 0;
    }
}


void clear_output()
{
}

char poll_input()
{
  getch();
  return SUCCESS;
}

void TRIS_init()
{
  BTN_TRIS = BTN_TRIS_MASK;
  DISPLAY_TRIS = DISPLAY_TRIS_MASK;
  EDIT_BUTTON_TRIS = EDIT_BUTTON_TRIS_MASK;

  TMR0_TRIS |= TMR0_TRIS_MASK;
  ADCON1 = 0x6;
}

char getval()
{
#if 1
  //tmp = getch();tmp = ((tmp & 0x30) >> 4) + 0x30;
  return ((char)(getch() & 0xff)) + 0x30;
#else
      return '8';
#endif
}

// #define DEBUG
int main()
{
  char tmp,tmp2;
#ifndef DEBUG
  #include "clock_main.c"
#else

  TRIS_init();

  clock_set_display(SEG7);
  while(true)
    {
      // 0xc3 = 1100 0011 <--- binary top
      // 0x43 = 0100 0011 <--- binary bottom
      // 0xa3 = 1010 0011 <--- right 7-seg
      // 0x03 = 0000 0011 <--- both 7-seg
      // 0x83 = 1000 0011 <--- right 7-seg
      // 0x13 = 0001 0011 <--- left 7-seg
      // RC7 = binary top not bottom (not left 7-seg)
      // RC6 = binary not 7-seg
      // RC4 = Not 7seg right (data with binary)
      //DISPLAY_PORT = SEG7_LEFT_INH_MASK | 0x03;
      //DISPLAY_PORT = DISPLAY_TYPE_MASK | BINARY_SIDE_MASK | 0x03;
      clock_set_display_side(LEFT);
      clock_set_digit(LEFT);
      tmp = (char)(getch() & 0xff);
      tmp2 = ((tmp & 0xf0) >> 4);
      if(tmp2 > 9)
	tmp2 += 'a' - 10;
      else
	tmp2 += '0';
      putch(tmp2);
      clock_set_digit(RIGHT);
      tmp2 = (tmp & 0xf);
      if(tmp2 > 9)
	tmp2 += 'a' - 10;
      else
	tmp2 += '0';
      putch(tmp2);
      clock_set_display_side(RIGHT);
      clock_set_digit(LEFT);
      tmp2 = ((tmp & 0xf0) >> 4);
      if(tmp2 > 9)
	tmp2 += 'a' - 10;
      else
	tmp2 += '0';
      putch(tmp2);
      clock_set_digit(RIGHT);
      tmp2 = (tmp & 0xf);
      if(tmp2 > 9)
	tmp2 += 'a'-10;
      else
	tmp2 += '0';
      putch(tmp2);
    }

  

#endif
}
