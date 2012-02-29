#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "7seg.h"
#include "input.h"
#include "utils.h"

#include <stddef.h>
#include <string.h>

static char display_side = LEFT, display_digit = RIGHT, display_type = SEG7;

char left_display_buffer[16];
char right_display_buffer[16];
char binary_display_buffer[2];
char real_7seg_output[5];

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


void set_display_data();

void putch_clock_display(char ch)
{
  update_display(clock_get_display_side(), ch);
  if(clock_get_display_side() == LEFT)
    clock_set_display_side(RIGHT);
  else
    clock_set_display_side(LEFT);
}

void clock_set_display(char type)
{
  switch(type)
    {
    case BINARY: case SEG7:
      display_type = type;
    default:
	break;
    }
}

char clock_get_display()
{
  return display_type;
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

void update_display(char side, char val)
{
  char *digits = real_7seg_output;
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
  
    set_display_data();
}

void set_display_data()
{ 
  char bit_counter, buffer_counter, high_nibble = 0, val;
  char *curr_buffer = NULL, *curr_word = NULL;// buffer is the actual DISPLAY_PORT values to be used. word is the human readable values to be converted to buffer.
  
  if(clock_get_display() == BINARY)
    {
      if(clock_get_display_side() == LEFT)// top
	{
	  high_nibble = DISPLAY_TYPE_MASK  | BINARY_SIDE_MASK;
	  curr_buffer = binary_display_buffer;
	}
      else
	{
	  high_nibble = DISPLAY_TYPE_MASK & ~BINARY_SIDE_MASK;
	  curr_buffer = binary_display_buffer + 1;
	}
      
      if(clock_get_digit() == LEFT)
	{
	  val <<=4;
	  val &= 0x3f;
	}
      *curr_buffer = val | high_nibble;
      return;
    }

  // 7seg
  if(clock_get_display_side() == LEFT)
    {
      high_nibble &= ~SEG7_LEFT_INH_MASK;
      high_nibble |= SEG7_RIGHT_INH_MASK;
      curr_buffer = left_display_buffer;
    }
  else
    {
      high_nibble &= ~SEG7_RIGHT_INH_MASK;
      high_nibble |= SEG7_LEFT_INH_MASK;
      curr_buffer = right_display_buffer;
    }
      
  high_nibble &= ~DISPLAY_TYPE_MASK;// sets 7-seg

  // Update buffer
  curr_word = real_7seg_output;
  if(curr_buffer == NULL || curr_word == NULL)//sanity check
    return;
  if(get_radix() == OCT)
    curr_word += 1;
  else if(get_radix() == DEC)
    curr_word += 3;

  memset(curr_buffer,0xff,16);

  buffer_counter = 0;
  for(;buffer_counter < 2;buffer_counter++)
    {
      if(buffer_counter == 0)//left first
	{
	  high_nibble |= SEG7_LEFT_DIGIT_MASK;
	  val = curr_word[0];
	}
      else
	{
	  high_nibble ^= SEG7_LEFT_DIGIT_MASK;
	  val = curr_word[1];// right digit
	}

      // Get 7seg bitmap
      if(val >= '0' && val <= '9')
	val -= '0';
      else if(val >= 'A' && val <= 'F')
	val = val - 'A'+10;
      else if(val >= 'a' && val <= 'f')
	val = val - 'a'+10;
      else
	val = 16;

      if(val > 16)
	val = 16;// failover to decimal
      val = table_7seg[val];
      
      bit_counter = 0;
      for(;bit_counter<8;bit_counter++,val >>= 1)
	{
	  if((val & 1) != 0)
	    {
	      *curr_buffer = (high_nibble | bit_counter);
	      curr_buffer += 1;
	    }
	}
    }
}





