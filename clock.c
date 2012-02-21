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
  GIE = 1;
  TMR0 = TMR0_PERIOD;

}

int getch()
{
  int button_buffer = BTN_PORT;
  char edit_button_val = 0;
  
  return button_buffer;
  
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

void set_display_data(char val)
{ 
  char counter, high_nibble = 0;
  
  if(clock_get_display() == BINARY)
    {
      if(clock_get_display_side() == LEFT)// top
	high_nibble |= BINARY_SIDE_MASK;
      else
	high_nibble &= ~BINARY_SIDE_MASK;
    }
  else
    {
      if(clock_get_display_side() == LEFT)
	{
	  high_nibble &= ~SEG7_LEFT_INH_MASK;
	  high_nibble |= SEG7_RIGHT_INH_MASK;
	}
      if(clock_get_digit() == LEFT)
	high_nibble |= SEG7_LEFT_DIGIT_MASK;
    }


  counter = 0;
  for(;counter<8;counter++,val >>= 1)
    {
      if((val & 1) != 0)
	{
	  PORTC = (high_nibble | counter);
	  __delay_ms(5);
	}
    }
}

void clear_output()
{
}

char poll_input()
{
  char buffer = BTN_PORT;
  char edit = (EDIT_BUTTON == 1), edit2;
  
  __delay_ms(20);

  button_state ^= buffer & BTN_PORT;
  
  edit2 = (EDIT_BUTTON == 1);
  edit_mode ^= edit & edit2;
  
  return SUCCESS;
}

void TRIS_init()
{
  BTN_TRIS = BTN_TRIS_MASK;
  DISPLAY_TRIS = DISPLAY_TRIS_MASK;
  EDIT_BUTTON_TRIS = EDIT_BUTTON_TRIS_MASK;
}

#define DEBUG
int main()
{

#ifndef DEBUG
  #include "clock_main.c"
#else

  TRIS_init();

  clock_set_display(SEG7);
  clock_set_display_side(LEFT);
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
      putch('1');
    }

  

#endif
}
