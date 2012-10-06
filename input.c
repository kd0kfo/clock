/**
 * Binary/Hex/Octal Clock
 * 
 * Firmware for  the Binary/Hex/Octal Clock
 * http://clock.davecoss.com
 *
 * Author: David Coss, PhD
 * Date: 6 Oct 2012
 *
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file has the code for the abstract IO functions.
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef LINUX_SIM
#include "curses.h"
#endif

#include "input.h"
#include "7seg.h"

#include "picos_time.h"
#include "picfs_error.h"

#include <stdbool.h>

char accumulator;
char radix = HEX;
char to_be_displayed = SHOW_TIME;

/**
 * Commands are the upper nibble of the char comm_arg parameter.
 * The lower nibble is the argument to the command
 */
signed char do_command(char comm_arg)
{
  signed char retval = SUCCESS;
  char command,arg;
  TIME_t the_time = *TIME_get();
  if((comm_arg & RUN_COMMAND_MASK) == 0)
    {
      arg = comm_arg & 0xf;
      if((comm_arg & MOVELWH_MASK) != 0)
	command = MOVELWH;
      else if((comm_arg & MOVELWL_MASK) != 0)
	command = MOVELWL;
    }
  else
    command = comm_arg & ~RUN_COMMAND_MASK;

  switch(command)
    {
    case MOVELWH:
      accumulator &= 0x0f;
      accumulator |= (arg << 4);
      break;
    case MOVELWL:
      accumulator &= 0xf0;
      accumulator |= arg;
      break;
    case SET_HOUR:
      the_time.hours = accumulator;
      TIME_set(&the_time);
      break;
    case SET_MINUTES:
      the_time.minutes = accumulator;
      TIME_set(&the_time);
      break;
    case SET_SECONDS:
      the_time.seconds = accumulator;
      TIME_set(&the_time);
      break;
    case SET_MONTH:
      the_time.month = accumulator;
      TIME_set(&the_time);
      break;
    case SET_DAY:
      the_time.day = accumulator;
      TIME_set(&the_time);
      break;
    case SET_YEAR:
      the_time.year = accumulator;
      TIME_set(&the_time);
      break;
    case USE_HEX:
      clock_set_display(SEG7);
      break;
    case USE_BINARY:
      clock_set_display(BINARY);
      break;
    case SHOW_DATE:case SHOW_TIME:
      to_be_displayed = command;
      break;
    case SET_RADIX:
      set_radix(accumulator);
      break;
    case GET_RADIX:
      accumulator = get_radix();
      break;
    default:
      retval = PICLANG_UNKNOWN_COMMAND;
      break;
    }

  return retval;
}

char should_do_command()
{
  extern char button_state;
  if((button_state & RUN_COMMAND_MASK) != 0 ||
    (button_state & MOVELWH_MASK) != 0 ||
     (button_state & MOVELWL_MASK) != 0)
    return true;
  
  return false;
}

char get_radix()
{
  return radix;
}

void set_radix(char new_radix)
{
  switch(new_radix)
    {
    case HEX:case OCT:case DEC:
      radix = new_radix;
    default:
      break;
    }
}

