#define STACK_LENGTH 8
#define LEFT_DISPLAY 0
#define RIGHT_DISPLAY 1

	cblock 0x45
stackHead:STACK_LENGTH
buffer_7seg:2
temp7seg
hours:1,minutes
alarmHours:1,alarmMinutes
dateMonth: 1,dateDay
count1:1,counta,countb
btn_state:1,btn_buffer,btn_pressed,btn_counter
	endc
#define END_OF_MEMORY btn_counter