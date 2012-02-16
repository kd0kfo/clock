#include "clock.h"

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

WINDOW *wnd = NULL, *seg7_wnd = NULL, *binary_wnd = NULL, *debug_wnd = NULL;

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
void putch(char c)
{ 
  putch_clock_display(c);
  
  
  //delch();
  box(lcd_wnd,ACS_VLINE,ACS_HLINE);
  if(c == 8)
    {
      refresh();
      return;
    }
  else if(c == 0x7f)
    {
      if(col == 0)
	{
	  col = TERM_WIDTH;
	  row = (row == 0) ? TERM_HEIGHT - 1: row - 1;
	}
      col--;
      wmove(lcd_wnd,row+row_offset,col+col_offset);
      wdelch(lcd_wnd);
      update_curses();
      return;
    }
  else if(c == '\n')
    {
      row++;
      if(row >= TERM_HEIGHT)
	row = 0;
      col = 0;
      wmove(lcd_wnd,row+row_offset,col+col_offset);
      update_curses();
      return;
    }
  wmove(lcd_wnd,row+row_offset,col+col_offset);
  wprintw(lcd_wnd,"%c",c);
  col++;
  if(col >= TERM_WIDTH)
    {
      row++;
      if(row >= TERM_HEIGHT)
	row = 0;
      col = 0;
    }
  wmove(lcd_wnd,row+col_offset,col+col_offset);
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
      if((curr_process.bitmap & PICLANG_BLOCKING_CALL) == 0)
	{
	  if(picos_curr_process >= 0 && picos_curr_process < PICOS_MAX_PROCESSES)
	    if(picos_processes[picos_curr_process].expires > 0)
	      picos_processes[picos_curr_process].expires--;
	  if(picos_wait_queue >= 0 && picos_wait_queue < PICOS_MAX_PROCESSES)
	    if(picos_processes[picos_wait_queue].expires > 0)
	      picos_processes[picos_wait_queue].expires--;
	}
      IN_INTERRUPT = 0;
    }
}

void interrupt_key_isr(int signal)
{
  signal_send(PICOS_SIGINT);
}

void TRIS_init(){}
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


FILE *sd_card, *fake_sram, *fake_eeprom;
void SPI_init(){}

void SD_read(picos_addr_t addr, void *buffer, char n)
{
  if(buffer == NULL)
    return;
  fseek(sd_card,addr,SEEK_SET);
  fread(buffer,sizeof(FS_Unit),n,sd_card);
}


void SD_write(picos_addr_t addr, void *buff, char size)
{
  if(buff == NULL)
    return;
  fseek(sd_card,addr,SEEK_SET);
  while(size > 0)
    {
      fwrite(buff++,sizeof(char),1,sd_card);
      size--;
    }
  fflush(sd_card);
}

void SRAM_read(unsigned int addr, void *buffer, char n)
{
  fseek(fake_sram,addr,SEEK_SET);
  fread(buffer,sizeof(FS_Unit),n,fake_sram);
}

void SRAM_write(unsigned int addr, const void *buffer, char n)
{
  fseek(fake_sram,addr,SEEK_SET);
  fwrite(buffer,sizeof(FS_Unit),n,fake_sram);
  fflush(fake_sram);
}

void startup_message()
{
  signed char fh = picfs_open("start",curr_dir);
  if(fh != -1)
    PICLANG_load((file_handle_t)fh);

  error_code = 0;
}

void main(void)
{
  /** LINUX SIM SETUP **/
  usart_init();
  wnd = initscr();
  noecho();
  keypad(wnd,true);
  mvwprintw(wnd,0,1,"LCD");
  lcd_wnd = subwin(wnd,TERM_HEIGHT+2,TERM_WIDTH+2,1,1);
  box(lcd_wnd,0,0);

  mvwprintw(wnd,0,TERM_WIDTH+5,"Core");
  core_wnd = subwin(wnd,CORE_HEIGHT,CORE_WIDTH,1,TERM_WIDTH+5);
  box(core_wnd,ACS_VLINE,ACS_HLINE);

  mvwprintw(wnd,CORE_HEIGHT+1,TERM_WIDTH+5,"Debug");
  debug_wnd = subwin(wnd,4,CORE_WIDTH,CORE_HEIGHT+2,TERM_WIDTH+5);
  box(debug_wnd,0,0);

  mvwprintw(wnd,0,TERM_WIDTH+10 + CORE_WIDTH,"Scheduler");
  wait_queue_wnd = subwin(wnd,SCHEDULER_HEIGHT,SCHEDULER_WIDTH,1,TERM_WIDTH+10 + CORE_WIDTH);
  box(wait_queue_wnd,0,0);

  mvwprintw(wnd,SCHEDULER_HEIGHT+1,TERM_WIDTH+10 + CORE_WIDTH,"PICFS Buffer");
  picfs_buffer_wnd = subwin(wnd,SCHEDULER_HEIGHT,SCHEDULER_WIDTH,SCHEDULER_HEIGHT+2,TERM_WIDTH+10+CORE_WIDTH);
  box(picfs_buffer_wnd,0,0);

  refresh();
  sd_card = fopen("sdcard","r+");
  if(sd_card == NULL)
    {
      fprintf(stderr,"Could not open fake sdcard\n");
      exit(errno);
    }

  fake_eeprom = fopen("fake_eeprom","r+");
  if(fake_eeprom == NULL)
    {
      fprintf(stderr,"Could not open fake eeprom\n");
      exit(errno);
    }

  remove("fake_sram");
  fake_sram = fopen("fake_sram","w+");
  if(fake_sram == NULL)
    {
      fprintf(stderr,"Could not open fake sram\n");
      exit(errno);
    }

  // Simulate interrupt key
  signal(SIGINT,interrupt_key_isr);

  /** END LINUX SIM STUFF **/
  #include "2550_main.inc.c"

  delwin(lcd_wnd);
  delwin(wnd);

  ARG_clear();
  endwin();
}
