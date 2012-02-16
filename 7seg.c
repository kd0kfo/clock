// PIC/SIM independent
extern bit use_hex_output;

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
 
  if(use_hex_output)
    {
      if(ch >= '0' && ch <= '9')
	set_display_data(table_7seg[ch-'0']);
      else if(ch >= 'A' && ch <= 'F')
	set_display_data(table_7seg[ch-'A']);
      else if(ch >= 'a' && ch <= 'f')
	set_display_data(table_7seg[ch-'a']);
      else
	set_display_data(table_7seg[16]);
    }
  else
    {
      if(ch >= '0' && ch <= '9')
	set_display_data(ch-'0');
      else if(ch >= 'A' && ch <= 'F')
	set_display_data(ch-'A');
      else if(ch >= 'a' && ch <= 'f')
	set_display_data(ch-'a');
      else
	set_display_data(0);
    }
}

