#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "picos_time.h"

#include <stdbool.h>

bit use_hex_output;

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



void interrupt isr()
{
  if(T0IF && T0IE)
    {
      TIME_tick();
      TMR0 = TMR0_PERIOD;
      T0IF = 0;
    }
}

void set_display_data(char val){ PORTC &= (val & 0xf) | 0xf0;}

char getch()
{
  return BTN_PORT;
}


int main()
{
  #include "clock_main.c"
}
