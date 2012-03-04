{
  const TIME_t *time = NULL;
  char last_time = 0;
  char input;

  TRIS_init();
  TIME_init();
  time = TIME_get();
  last_time = time->minutes - 1;

  //clock_set_display(BINARY);
  clock_set_display(SEG7);

  while(true)
    {
      if(last_time != time->minutes)
	{
	  if(to_be_displayed == SHOW_TIME)
	    {
	      update_display(LEFT,time->hours);
	      update_display(RIGHT, time->minutes);
	    }
	  else if(to_be_displayed == SHOW_DATE)
	    {
	      update_display(LEFT, time->month);
	      update_display(RIGHT, time->day);
	    }
	  last_time = time->minutes;
	}// END CLOCK DISPLAY UPDATE

      refresh_display();


	  if(EDIT_BUTTON)
	    {
	      char last_button_state = 0xff;
	      last_button_state--;
	      poll_input();
	      clear_output();
	      while(edit_mode)
		{

		  if(should_do_command())
		    {
		      do_command(button_state);
		      while(should_do_command() && edit_mode)
			{
			  poll_input();
			  continue;
			}
		    }
		    
		  if(last_button_state != button_state)
		    {
		      update_display(RIGHT, button_state);
		      update_display(LEFT, accumulator);
		    }
		  refresh_display();
		  poll_input();
		  continue;
		}// END EDIT INPUT WHILE LOOP
	    }// END EDIT MODE
    }// INFINITE LOOP
}// MAIN BLOCK
