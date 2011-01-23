#define STACK_LENGTH:8

cblock 0x45
	stackHead:STACK_LENGTH
	display_buffer:left,right
	time:hours,minutes
	alarm_time:alarmHours,alarmMinutes
	date:dateMonth,dateDay
endc