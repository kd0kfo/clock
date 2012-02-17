#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef LINUX_SIM
#include "curses.h"
#endif

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
  val = 1 << val;
  
  button_state ^= (char)(val & 0xff);
  
}
