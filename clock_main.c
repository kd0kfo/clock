{
  const TIME_t *time = NULL;
  char digits[2];
  char last_time = 0;
  char input;

  TIME_init();
  time = TIME_get();
  last_time = time->seconds;

  clock_set_display(BINARY);
  //clock_set_display(SEG7);

  while(true)
    {
      if(last_time != time->seconds)
	{
	  if(to_be_displayed == SHOW_TIME)
	    {
	      clock_set_display_side(LEFT);
	      hex_to_word(digits, time->hours);
	      clock_write_double_digit(digits);
	  
	      clock_set_display_side(RIGHT);
	      hex_to_word(digits, time->minutes);
	      clock_write_double_digit(digits);
	    }
	  else if(to_be_displayed == SHOW_DATE)
	    {
	      clock_set_display_side(LEFT);
	      hex_to_word(digits, time->month);
	      clock_write_double_digit(digits);
	  
	      clock_set_display_side(RIGHT);
	      hex_to_word(digits, time->day);
	      clock_write_double_digit(digits);
	    }
	  
	  poll_input();

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
		    
		  
		  clock_set_display_side(LEFT);
		  hex_to_word(digits,accumulator);
		  clock_write_double_digit(digits);
		  
		  clock_set_display_side(RIGHT);
		  hex_to_word(digits,button_state);
		  clock_write_double_digit(digits);
		  poll_input();
		  continue;
		}// END EDIT INPUT WHILE LOOP
	    }// END EDIT MODE
	}// END CLOCK DISPLAY UPDATE
    }// INFINITE LOOP
}// MAIN BLOCK
