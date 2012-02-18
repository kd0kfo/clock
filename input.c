#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef LINUX_SIM
#include "curses.h"
#endif

#include "picfs_error.h"


signed char do_command(char command)
{
  signed char retval = SUCCESS;
  switch(command)
    {
    default:
      retval = PICLANG_UNKNOWN_COMMAND;
      break;
    }

  return retval;
}

