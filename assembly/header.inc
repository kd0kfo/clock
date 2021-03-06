	
	list p=16f870
	include <p16f870.inc>
	radix hex

	__config _WDT_OFF

#include "mem.inc"
#include "../../picos/globals.inc"
#include "../../picos/kernel.inc"
#include "../../picos/kernel_memory.inc"
#include "../../picos/stack.inc"
#include "../../picos/io.inc"
#include "../../picos/time.inc"


#define outport PORTC
#define outportTRIS TRISC
#define controlPort PORTA
#define controlPortTRIS TRISA
#define dipControl PORTB
#define dipControlTRIS TRISB

#define EDIT_BUTTON PORTA,0

#define BTN_PORT PORTB
#define BTN1 0
#define BTN2 1
#define BTN3 2
#define BTN4 3
#define BTN5 4
#define BTN6 5
#define BTN7 6
#define BTN8 7

;In the follwing bit definitions, the first word/phrase indicates
;the meaning of Logic 1 and the second word/phrase indicates
;the meaning of Logic 0
;
;myStatus bits
#define HOUR_MIN_BIT 0
#define BINARY_7SEG_BIT 1
;N stands for nibble
#define LSN_MSG_7SEG_BIT 2
#define ALARM_ON_OFF_BIT 3
#define HEX_OCTAL_BIT 4
#define ALARM_FLAG_TOGGLE 0x8
	
;DIP input port bits
#define DISPLAY_DATE_BIT 0
#define DISPLAY_ALARM_BIT 1
#define INPUT_BIT 7

;control port bits
#define PROGRAM_MODE_BIT 0
#define ALARM_TRIGGER_BIT 1
#define TIMER0_CLK 4
#define INITIAL_TIMER_VALUE 0xc4

;time stuff
#define RESET_CLOCK_VALUE 0xc4
#define INCREMENT_TIME_INTERRUPT INTCON,T0IF
#define CLOCK_TIMER_FLAG INTCON,2

;commands
#define SET_MINUTES_CODE 0
#define SET_HOURS_CODE 1
#define SET_ALARM_MINUTES_CODE 2
#define SET_ALARM_HOURS_CODE 3
#define SET_ALARM_DAY_CODE 4
#define SET_DAY_CODE 5
#define SET_MONTH_CODE 6
#define TOGGLE_ALARM_CODE 7
#define SHOW_DATE_CODE 0x8
#define TOGGLE_HEX_OCTAL_CODE 0x10
