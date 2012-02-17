{
  const TIME_t *time = NULL;
  char digits[2];
  char last_time = 0;

  TIME_init();
  time = TIME_get();
  last_time = time->seconds;
  use_hex_output = true;

  while(true)
    {
      if(last_time != time->seconds)
	{
	  clock_set_display_side(LEFT);
	  hex_to_word(digits, time->minutes);
	  clock_write_double_digit(digits);

	  clock_set_display_side(RIGHT);
	  hex_to_word(digits, time->seconds);
	  clock_write_double_digit(digits);
	  
	}
    }
}
