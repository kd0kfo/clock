#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "7seg.h"
#include "input.h"
#include "utils.h"

#include <stddef.h>

static char display_side = LEFT, display_digit = RIGHT;

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


void putch_clock_display(char ch)
{
  extern void set_display_data(char val);

  if(clock_get_display() == SEG7)
    {
      if(ch >= '0' && ch <= '9')
	set_display_data(table_7seg[ch-'0']);
      else if(ch >= 'A' && ch <= 'F')
	set_display_data(table_7seg[ch-'A'+10]);
      else if(ch >= 'a' && ch <= 'f')
	set_display_data(table_7seg[ch-'a'+10]);
      else
	set_display_data(table_7seg[16]);
    }
  else
    {
      if(ch >= '0' && ch <= '9')
	set_display_data(ch-'0');
      else if(ch >= 'A' && ch <= 'F')
	set_display_data(ch-'A'+10);
      else if(ch >= 'a' && ch <= 'f')
	set_display_data(ch-'a'+10);
      else
	set_display_data(0);
    }
}

void clock_set_display(char type)
{
  if(type == BINARY)
    DISPLAY_PORT |= DISPLAY_TYPE_MASK;
  else
    DISPLAY_PORT &= ~DISPLAY_TYPE_MASK;

}

char clock_get_display()
{
  if((DISPLAY_PORT & DISPLAY_TYPE_MASK) == 0)
    return SEG7;
  
  return BINARY;
  
}

void clock_set_display_side(char display)
{
  switch(display)
    {
    case LEFT: case RIGHT:
      display_side = display;
    default:
      break;
    }
}

char clock_get_display_side()
{
  return display_side;
}

void clock_set_digit(char digit)
{
  switch(digit)
    {
    case LEFT: case RIGHT:
      display_digit = digit;
    default:
      break;
    }
}

char clock_get_digit()
{
  return display_digit;
}

void clock_write_double_digit(const char *digits)
{
  if(digits == NULL)
    return;
  
  clock_set_digit(LEFT);
  putch(digits[0]);
  clock_set_digit(RIGHT);
  putch(digits[1]);
}

void update_display(char side, char val)
{
  char digits[5];
  char *write_addr = digits;
  char radix;
  extern char get_radix();
  
  clock_set_display_side(side);
  // Override radix for binary output
  if(clock_get_display() == BINARY)
    {
      hex_to_word(digits, val);
    }
  else
    {
      switch(get_radix())
	{
	case DEC:
	  dec_to_word(digits, val);
	  write_addr += 3;
	  break;
	case OCT:
	  oct_to_word(digits, val);
	  write_addr += 1;
	  break;
	case HEX:default:
	  hex_to_word(digits, val);
	  break;
	}
    }

  clock_write_double_digit(write_addr);

}



