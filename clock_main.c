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
	  poll_input();
	}// END CLOCK DISPLAY UPDATE

      refresh_display();

      

	  if(edit_mode)
	    {
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
		    
		  
		  update_display(LEFT, accumulator);
		  update_display(RIGHT, button_state);

		  poll_input();
		  continue;
		}// END EDIT INPUT WHILE LOOP
	    }// END EDIT MODE
    }// INFINITE LOOP
}// MAIN BLOCK
