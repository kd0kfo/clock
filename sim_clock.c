#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "7seg.h"
#include "input.h"

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

WINDOW *wnd = NULL, *seg7_wnd = NULL, *binary_wnd = NULL, *button_wnd = NULL;
#define SEG7_WIDTH  25
#define SEG7_HEIGHT  5
#define BINARY_WIDTH  20
#define BINARY_HEIGHT  5

void update_buttons()
{
  char button_buffer[8];
  
  binary_to_octuplet(button_buffer,button_state);
  mvwprintw(button_wnd,1,1,"%s  %c",button_buffer,((edit_mode)?'1':'0'));
  
}


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
  
  update_buttons();
  
  
  touchwin(wnd);
  refresh();
}
void IO_flush(){}
void clear_output()
{
  wclear(seg7_wnd);
  wclear(binary_wnd);
}

char poll_input()
{
  int val = getch();
  if(val == ERR)
    return button_state;
  
  if(val == 0xa)
    {
      EDIT_BUTTON ^= 1;
      if(EDIT_BUTTON)
	edit_mode = 1;
      else
	edit_mode = 0;
      return button_state;
    }

  if(val < '1' || val > '8')
    return button_state;
  
  val -= '1';
  val = 0x80 >> val;
  
  button_state ^= (char)(val & 0xff);
  
}


char display_data = 0;
void set_display_data(char val){display_data = val;}
void putch(char c)
{ 
  int curr_line = 1,curr_column = 2;
  putch_clock_display(c);
  

  //delch();
  //wclear(seg7_wnd);
  if(clock_get_display() == SEG7)
    {
      wclear(binary_wnd);
      box(seg7_wnd,ACS_VLINE,ACS_HLINE);
      wmove(seg7_wnd, curr_line,1);
      if(clock_get_display_side() == RIGHT)
	curr_column += 12;
      if(clock_get_digit() == RIGHT)
	curr_column += 5;
      if((display_data & 0x40) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column," --- ");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column,"     ");
      curr_line++;

      if((display_data & 0x20) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column,"|");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column," ");
      if((display_data & 0x02) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column+4,"|");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column+4," ");
      curr_line++;

      if((display_data & 0x80) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column," --- ");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column,"     ");
      curr_line++;

      if((display_data & 0x10) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column,"|");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column," ");
      if((display_data & 0x04) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column+4,"|");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column+4," ");
      curr_line++;

      if((display_data & 0x08) != 0)
	mvwprintw(seg7_wnd,curr_line,curr_column," --- ");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column,"     ");
      curr_line++;

#if 0// do not use 7-seg  
      if(display_data == 1)
	mvwprintw(seg7_wnd,curr_line,curr_column+4,".");
      else
	mvwprintw(seg7_wnd,curr_line,curr_column+4," ");
#endif
    }
  else
    {
      char binary_buffer[8];
      wclear(seg7_wnd);
      binary_to_octuplet(binary_buffer,display_data);
      if(clock_get_display_side() == RIGHT)
	curr_line++;
      if(clock_get_digit() == RIGHT)
	curr_column += 4;

      mvwprintw(binary_wnd,curr_line,curr_column,"%s",&binary_buffer[4]);
      
    }
  
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
  delwin(seg7_wnd);
  delwin(binary_wnd);
  delwin(wnd);

  endwin();
  exit(0);

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
  nodelay(wnd,true);
  
  noecho();
  keypad(wnd,true);
  mvwprintw(wnd,0,1,"7seg");
  seg7_wnd = subwin(wnd,SEG7_HEIGHT+2,SEG7_WIDTH+2,1,1);
  box(seg7_wnd,0,0);

  mvwprintw(wnd,0,SEG7_WIDTH+5,"Binary");
  binary_wnd = subwin(wnd,BINARY_HEIGHT,BINARY_WIDTH,1,SEG7_WIDTH+5);
  box(binary_wnd,ACS_VLINE,ACS_HLINE);

  mvwprintw(wnd,SEG7_HEIGHT+3,1,"Buttons");
  button_wnd = subwin(wnd,4,SEG7_WIDTH,SEG7_HEIGHT+4,1);
  box(button_wnd,ACS_VLINE,ACS_HLINE);

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

}
