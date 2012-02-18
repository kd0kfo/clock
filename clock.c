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
  char counter;
  counter = 0;
  for(;counter<8;counter++,val >>= 1)
    {
      if((val & 1) != 0)
	PORTC &= (0xf8 | counter);
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

int main()
{
  #include "clock_main.c"
}
