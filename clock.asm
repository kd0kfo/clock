	;
	;clock software 1.0
	;Author: David Coss
	;Copyright: Free to use with attribution to the Author. 
	;
	; See bottom for description of changes per version.
	;
#include "header.inc"
#include "mem.inc"
#include "../../picos/globals.inc"
#include "../../picos/kernel_memory.inc"
#include "../../picos/kernel.inc"
#include "../../piclang/piclang.inc"
#include "../../picos/bin_7seg.inc"
#include "../../picos/stack.inc"
#include "../../picos/time.inc"
#include "../../picos/version.inc"

	org 0x00
	goto INIT
	;
	org 0x04
	START_INTERRUPT saveW,interruptBankSave,bankSelection,SAVE_BANK
	btfsc INCREMENT_TIME_INTERRUPT
	goto ISR_INCREMENT_TIME_OF_DAY
	;more interrupt stuff can go here
END_OF_INTERRUPT nop
	FINISH_INTERRUPT saveW,interruptBankSave,bankSelection,RESET_BANK
	retfie
INIT INIT_KERNEL_MAC errorByte 
	INIT_MEMORY_MAC endOfMemory;init stack
	INIT_STACK_MAC stackHead,stackPtr
	;
	;init display
	INIT_DISPLAY_MAC firstDisplay,lastDisplay,indicator
	movlw INITIAL_TIMER_VALUE
	movwf resetTMR0;
	;
	;initialize ports
	movlw b'10111'
	movwf myStatus;default hour, 7-seg, right display, alarm off, use hex
	;bit 0 = output Hour/~output Minute. bit 1 = ~7-seg/binary display. bit 2 = right 7-seg/~left 7-seg. bit 3 = alarm on/~off
	;
	bcf STATUS,RP0
	bcf STATUS,RP1;
	clrf outport
	clrf controlPort
	clrf dipControl
	bsf STATUS,RP0;bank 1
	movlw 0x0; all output
	movwf outportTRIS; Hours/~Minutes(7bit), Binary/~7-seg(6bit), When 7-seg is on bit 4 is ~minutes and bit 7 is ~hour
	movlw 0x6;
	movwf ADCON1;SET bit 0 of porta to be digital (not analog :-/ )
	movlw b'00010001';TIMER MUST BE INPUT!!!
	movwf controlPortTRIS;bit 0 = Program Mode On, bit 1 = external alarm trigger, 4 = timer0 clock
	movlw b'11111111';Oh, yeah. dip switch keyboard
	movwf dipControlTRIS;bit 7 = trigger for dip switch commands.
	bsf OPTION_REG,5;turn on timer0
	bcf OPTION_REG,4;time on leading edge of T0CKI
	bcf STATUS,RP0
	;
	bcf STATUS,0       ; Clear carry bit
	bcf STATUS,2       ; Clear zero flag
	bcf STATUS,1       ;
	bsf INTCON,5       ; Enable timer0 interrupt
	bcf INTCON,2       ; Clear interrupt flag
	bsf INTCON,7       ; Enable global interrupt
	movf resetTMR0,W
	movwf TMR0; Count from c4 to overflow (60secs) then interrupt
	;
	movlw 0x40
	movwf eepromSize
	movlw .0
	movwf output
	movlw .29
	movwf dateDay
	movlw 0x6
	movwf dateMonth
	movlw 0x00;start at midnight
	movwf hours
	movwf minutes
	movwf alarmMinutes
	movwf alarmHours;off by default. cf mystatus.
	bcf controlPort,1
	movlw .0
	movwf counter
	;
MAIN_LOOP call SHOW_CLOCK
	MAIN_PROCESS_MAC CREATE_DISPLAY,DISPLAY_ME,controlPort,PROGRAM_MODE,MAIN_LOOP;generic process loop from kernel.asm

;bin 7-seg display macros
CREATE_DISPLAY_MAC myStatus,minutes,hours,MAKE_PACKET,binaryMinute,binaryHours,controlPort,rightDisplay,SEG_VALUES,output,LOAD_PACKET,indicator,hexToOctal

	;
ERROR_RETURN nop;not sure what to do in case of error yet
	return
	;
	;macro calls

	;
	END

	;1.1: Added multiple opcodes.
	;1.0:	added defines. Added stack. stackPtr will always
	;		contain the "top" of the stack. Therefore,
	;		popping the stack means:
	;			movf stackPtr,W
	;			movwf FSR
	;			decf stackPtr,F
	;		INDF will then contain the data that has
	;		been popped. Pushing into the stack is
	;		done by:
	;			incf stackPtr,F
	;			movwf INDF
	;			
	;0.8:	Added Date!
	;0.7:	Added alarm clock function. Also, I changed
	;		the way time is set. Now, it uses FSR to
	;		set the hours and minutes one at a time.
	;		This also allows me to use the same routine
	;		for the alarm and the time and it gives me more
	;		command options (and saving program memory :-D )
	;
	;0.6.0: Completely changed setup to *store* binary & 
	;		7-seg packets so that more time can be spent
	;		lighting them.
	;0.5.1: Fixed rotation error in switching between hex
	;		and octal outputs
	;0.5.0:	Added controls to set time using PORTB connected
	;		to a 8-bit dip switch.
	;0.4.4: Fixed the left 7-seg display for octal output.
	;0.4.3: Added the switch to change PORTA from analog to 
	;		digital input.
	;0.4.2: Fixed 7-seg ata table values. Changed start time 
	;       to 0:00. Fixed a mistake which prevented the hex
	;		display flag from being set with minutes. Set bit
	;		0 of PORTA to input (hex/~octal flag)
	;0.4.1: Added comments to understand which bits control 
	;		specific LED's in the output. Set TMR0 to input
	;		and all other PORTA bits to output. Time starts
	;		at 5:05am so that I can test different bits and
	;		output at startup.
