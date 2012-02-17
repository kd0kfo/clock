#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "picfs_error.h"
#include "scheduler.h"
#include "picos_time.h"
#include "version.h"

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <math.h>

#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

bit use_hex_output;
WINDOW *wnd = NULL, *seg7_wnd = NULL, *binary_wnd = NULL, *debug_wnd = NULL;
#define SEG7_WIDTH  10
#define SEG7_HEIGHT  10
#define BINARY_WIDTH  10
#define BINARY_HEIGHT  10

void update_curses()
{
  /*  wclear(picfs_buffer_wnd);
  wclear(core_wnd);
  box(wait_queue_wnd,ACS_VLINE,ACS_HLINE);
  box(picfs_buffer_wnd,ACS_VLINE,ACS_HLINE);
  box(debug_wnd,ACS_VLINE,ACS_HLINE);
  core_to_curses();
  scheduler_to_curses();
  buffer_to_curses();*/
  touchwin(wnd);
  refresh();
}
void IO_flush(){}
void clear_output()
{
  update_curses();
}


char display_data = 0;
void set_display_data(char val){}
void putch(char c)
{ 
  putch_clock_display(c);
  
  
  //delch();
  box(seg7_wnd,ACS_VLINE,ACS_HLINE);
  wprintw(seg7_wnd,"%c",c);
  update_curses();

}

bit IN_INTERRUPT = 0;
/*void interrupt isr(void)*/
void interrupt_isr(union sigval arg)
{
  // Clock code
  if(IN_INTERRUPT == 0)
    {
      IN_INTERRUPT = 1;
      TIME_tick();
      IN_INTERRUPT = 0;
    }
}

void interrupt_key_isr(int signal)
{
}

void TRIS_init(){}

FILE *usart_fifo = NULL;
void usart_init()
{  
  usart_fifo = fopen("usart_fifo","w+");
  if(usart_fifo == NULL)
    {
      fprintf(stderr,"WARNING: Could not open fake usart file\n");
    }

}

timer_t timerID;
struct sigevent se;
struct itimerspec ts= {{1,0},{1,0}};
void TIME_init()
{
  memset(&se,0,sizeof(se));
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = "tick";
  se.sigev_notify_function = interrupt_isr;
  se.sigev_notify_attributes = NULL;

  if (timer_create(CLOCK_REALTIME, &se, &timerID) != 0)
    {
      perror("Error creating timer.");
      exit( -1);
    }
  
  
  if (timer_settime(timerID, 0, &ts, NULL) != 0)
    {
      perror("Error setting up timer.");
      exit(-1);
    }
  TIME_t new_time;
  new_time.hours = 12;
  new_time.minutes = 34;
  new_time.seconds = 56;
  new_time.month = 6;
  new_time.day = 29;
  new_time.year = 0;

  TIME_set(&new_time);


}


FILE *fake_eeprom;
void tone_440(){}

void main(void)
{
  /** LINUX SIM SETUP **/
  usart_init();
  wnd = initscr();
  noecho();
  keypad(wnd,true);
  mvwprintw(wnd,0,1,"7seg");
  seg7_wnd = subwin(wnd,SEG7_HEIGHT+2,SEG7_WIDTH+2,1,1);
  box(seg7_wnd,0,0);

  mvwprintw(wnd,0,SEG7_WIDTH+5,"Binary");
  binary_wnd = subwin(wnd,BINARY_HEIGHT,BINARY_WIDTH,1,SEG7_WIDTH+5);
  box(binary_wnd,ACS_VLINE,ACS_HLINE);

  refresh();
  fake_eeprom = fopen("fake_eeprom","r+");
  if(fake_eeprom == NULL)
    {
      fprintf(stderr,"Could not open fake eeprom\n");
      exit(errno);
    }

  // Simulate interrupt key
  signal(SIGINT,interrupt_key_isr);

  /** END LINUX SIM STUFF **/
  #include "clock_main.c"

  delwin(seg7_wnd);
  delwin(binary_wnd);
  delwin(wnd);

  endwin();
}
