	;
	;clock software 1.0
	;Author: David Coss
	;Copyright: Free to use with attribution to the Author. 
	;
	; See bottom for description of changes per version.
	;
#include "header.inc"

	org 0x00
	goto INIT
	;
	org 0x04
ISR_INCREMENT_TIME_OF_DAY_MAC TMR0,myStatus,alarmHours,hours,alarmMinutes,minutes,INC_MINUTES,END_OF_INTERRUPT
END_OF_INTERRUPT 
	retfie
INTERRUPT_CHECKED_TIME return

	;
	
INIT 
	INIT_STACK_MAC stackHead,stackPtr
	movlw 0x0
	movwf low buffer_7seg + RIGHT_DISPLAY
	movwf low buffer_7seg + LEFT_DISPLAY
	movlw 0x29
	movwf accumulator
	bcf STATUS,RP0
	bcf STATUS,RP1;
	;movf resetTMR0,W
	movlw 0xc4
	movwf TMR0; Count from c4 to overflow (60secs) then interrupt
	clrf controlPort
	clrf dipControl
	clrf outport
	bsf STATUS,RP0;bank 1
	movlw 0x0; all output
	movwf outportTRIS; Hours/~Minutes(7bit), Binary/~7-seg(6bit), When 7-seg is on bit 4 is ~minutes and bit 7 is ~hour
	movlw 0x6;
	movwf ADCON1;SET bit 0 of porta to be digital (not analog :-/ )
	movlw b'00010001';TIMER MUST BE INPUT!!!
	movwf TRISA;bit 0 = hex/~octal, bit 1 = external alarm trigger, 4 = timer0 clock
	movlw b'11111111';Oh, yeah. dip switch keyboard
	movwf dipControlTRIS;bit 7 = trigger for dip switch commands.
	bsf OPTION_REG,T0CS;turn on timer0
	bcf OPTION_REG,T0SE;time on leading edge of T0CKI
	bcf STATUS,RP0
	;
	bcf STATUS,0       ; Clear carry bit
    bcf STATUS,2       ; Clear zero flag
    bcf STATUS,1       ;
    bsf INTCON,T0IE       ; Enable timer0 interrupt
    bcf INTCON,2       ; Clear interrupt flag
    bsf INTCON,7       ; Enable global interrupt
    call BANK_0
	clrf btn_state		; it's in the same bank as seconds :-/
	clrf btn_counter
	clrf btn_buffer
	clrf btn_pressed
	clrf minutes
	clrf hours

	bsf myStatus,HEX_OCTAL_BIT
	
MAIN_LOOP movf minutes,W
	movwf buffer_7seg + RIGHT_DISPLAY
	movf hours,W
	movwf buffer_7seg + LEFT_DISPLAY
	call DISPLAY_BINARY
	call DISPLAY_7SEG
	call DEBOUNCE_BUTTONS
	btfsc EDIT_BUTTON
	goto EDIT_MODE
	goto MAIN_LOOP

EDIT_MODE movf accumulator,W
	movwf buffer_7seg + RIGHT_DISPLAY
	call DISPLAY_7SEG
	call DEBOUNCE_BUTTONS
	btfsc dipControl,BTN8
	goto MOV_INPUT_ACC
UPDATE_EDIT_DISPLAY bcf dipControl,BTN8
	movf dipControl,W
	movwf buffer_7seg + LEFT_DISPLAY
	btfss EDIT_BUTTON
	goto MAIN_LOOP
	goto EDIT_MODE
MOV_INPUT_ACC movf dipControl,W
	andlw 0x7f
	movwf accumulator
	movwf buffer_7seg + RIGHT_DISPLAY
	movlw SET_MINUTES_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_MINUTES
	movlw SET_HOURS_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_HOURS
	movlw SET_ALARM_HOURS_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_ALARM_HOURS
	movlw SET_ALARM_MINUTES_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_ALARM_MINUTES
	movlw SET_DAY_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_DAY
	movlw SET_MONTH_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SET_MONTH
	movlw TOGGLE_ALARM_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto TOGGLE_ALARM
	movlw SHOW_DATE_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto SHOW_DATE
	movlw TOGGLE_HEX_OCTAL_CODE
	xorwf accumulator,W
	btfsc STATUS,Z
	goto TOGGLE_HEX_OCTAL_BIT
UNKNOWN_EDIT_CODE 
EDIT_MODE_NEXT clrf accumulator
	goto EDIT_MODE

TOGGLE_HEX_OCTAL_BIT movf myStatus,W
	movwf temp
	btfsc myStatus,HEX_OCTAL_BIT
	bcf temp,HEX_OCTAL_BIT
	btfss myStatus,HEX_OCTAL_BIT
	bsf temp,HEX_OCTAL_BIT
	movf temp,W
	call PUSH_STACK
	movlw 0x8
	btfsc temp,HEX_OCTAL_BIT
	movlw 0x16
	bsf myStatus,HEX_OCTAL_BIT ; so that we can read the hex in W-reg like it was decimal.
	movwf buffer_7seg + RIGHT_DISPLAY
	clrf buffer_7seg + LEFT_DISPLAY
TOGGLE_HEX_OCTAL_BIT_LOOP call DISPLAY_7SEG
	btfsc dipControl,BTN8
	goto TOGGLE_HEX_OCTAL_BIT_LOOP
	call POP_STACK
	movwf myStatus
	goto EDIT_MODE_NEXT

SET_MINUTES movlw low minutes
	call SET_MEMORY
	goto EDIT_MODE_NEXT

SET_HOURS movlw low hours
	call SET_MEMORY
	goto EDIT_MODE_NEXT

SET_ALARM_MINUTES movlw low alarmMinutes
	call SET_MEMORY
	goto EDIT_MODE_NEXT

SET_ALARM_HOURS movlw low alarmHours
	call SET_MEMORY
	goto EDIT_MODE_NEXT

SET_DAY movlw low dateDay
	call SET_MEMORY
	goto EDIT_MODE_NEXT
	
SET_MONTH movlw low dateMonth
	call SET_MEMORY
	goto EDIT_MODE_NEXT

SET_MEMORY movwf FSR
	movf INDF,W
	movwf buffer_7seg + LEFT_DISPLAY
	call DISPLAY_7SEG
	call Delay1s
SET_MEMORY_LOOP movf dipControl,W
	andlw 0x7f
	movwf buffer_7seg + LEFT_DISPLAY
	call DISPLAY_7SEG
	btfss dipControl,BTN8
	goto SET_MEMORY_LOOP
	movf buffer_7seg + LEFT_DISPLAY,W
	movwf INDF
	goto Delay1s
	
TOGGLE_ALARM movf alarmHours,W
	movwf buffer_7seg + LEFT_DISPLAY
	movf alarmMinutes,W
	movwf buffer_7seg + RIGHT_DISPLAY
	btfsc myStatus,ALARM_ON_OFF_BIT
	goto CLEAR_ALARM_BIT
	bsf myStatus,ALARM_ON_OFF_BIT
	goto END_TOGGLE_ALARM
CLEAR_ALARM_BIT bcf myStatus,ALARM_ON_OFF_BIT	
END_TOGGLE_ALARM call DISPLAY_7SEG
	btfsc dipControl,BTN8
	goto END_TOGGLE_ALARM
	goto EDIT_MODE_NEXT

SHOW_DATE movf dateMonth,W
	movwf buffer_7seg + LEFT_DISPLAY
	movf dateDay,W
	movwf buffer_7seg + RIGHT_DISPLAY
END_SHOW_DATE call DISPLAY_7SEG
	btfsc dipControl,BTN8
	goto END_SHOW_DATE
	goto EDIT_MODE_NEXT



;bin 7-seg display macros
;CREATE_DISPLAY_MAC myStatus,minutes,hours,MAKE_PACKET,binaryMinute,binaryHours,controlPort,rightDisplay,SEG_VALUES,output,LOAD_PACKET,indicator,hexToOctal
INCREMENT_TIME_MAC dateDay,dateMonth,minutes,hours,ISR_REST_TMR,NUMBER_OF_DAYS
NUMBER_OF_DAYS_MAC pclTemp
	;
ERROR_RETURN clrf errorByte;not sure what to do in case of error yet
	return
	;
	;macro calls
STACK_MAC stackTemp,stackTemp2,stackPtr
BANK_MASK_MAC bankSelection

DISPLAY_BINARY swapf buffer_7seg + RIGHT_DISPLAY,W
	andlw 0xf
	call PUSH_STACK
	movlw LEFT_DISPLAY
	call PUSH_STACK
	call DISPLAY_BINARY_NIBBLE
	movf buffer_7seg + RIGHT_DISPLAY,W
	andlw 0xf
	call PUSH_STACK
	movlw RIGHT_DISPLAY
	call PUSH_STACK
	call DISPLAY_BINARY_NIBBLE
	return

DISPLAY_BINARY_NIBBLE call POP_STACK
	xorlw LEFT_DISPLAY
	btfsc STATUS,Z
	bsf outport,7
	btfss STATUS,Z
	bcf outport,7
	bsf outport,6
	movlw 0xC0
	andwf outport,F
	call POP_STACK
	addwf outport,F
	return;goto Delay20

DISPLAY_7SEG 
DISPLAY_7SEG_RIGHT
	movf buffer_7seg + RIGHT_DISPLAY,W
	andlw 0xf
	btfss myStatus,HEX_OCTAL_BIT
	andlw 0x7
	call PUSH_STACK ; value to display
	movlw RIGHT_DISPLAY ; digit
	call PUSH_STACK
	movlw RIGHT_DISPLAY ; 7seg
	call PUSH_STACK
	call DISPLAY_7SEG_NIBBLE
	movlw 0xf0
	btfss myStatus,HEX_OCTAL_BIT
	movlw b'111000'
	andwf buffer_7seg+RIGHT_DISPLAY,W
	movwf temp7seg
	bcf STATUS,C
	rrf temp7seg,F
	rrf temp7seg,F
	rrf temp7seg,F
	btfsc myStatus,HEX_OCTAL_BIT
	rrf temp7seg,F
	movf temp7seg,W
	call PUSH_STACK
	movlw LEFT_DISPLAY
	call PUSH_STACK
	movlw RIGHT_DISPLAY
	call PUSH_STACK
	call DISPLAY_7SEG_NIBBLE
DISPLAY_7SEG_LEFT
	movf buffer_7seg + LEFT_DISPLAY,W
	andlw 0xf
	call PUSH_STACK ; value to display
	movlw RIGHT_DISPLAY ; left digit
	call PUSH_STACK
	movlw LEFT_DISPLAY ; left 7seg
	call PUSH_STACK
	call DISPLAY_7SEG_NIBBLE
	swapf buffer_7seg+LEFT_DISPLAY,W
	andlw 0xf
	call PUSH_STACK
	movlw LEFT_DISPLAY
	call PUSH_STACK
	movlw LEFT_DISPLAY
	call PUSH_STACK
	call DISPLAY_7SEG_NIBBLE
	return	

;Displays a nibble on the set of 7-seg displays
; Left and right sides are determined using the
; "LEFT_DISPLAY" and "RIGHT_DISPLAY" define values.
;
;Arguments: From top of stack downwards:
;	Top)  Left or Right pair of displays
;   Next) Left Digit or Right Digit
;	Next) Nibble to be displayed.
DISPLAY_7SEG_NIBBLE clrf temp7seg
	call POP_STACK
	xorlw LEFT_DISPLAY	
	btfss STATUS,Z
	bsf temp7seg,7
	btfsc STATUS,Z
	bsf temp7seg,4
	call POP_STACK
	xorlw RIGHT_DISPLAY
	btfss STATUS,Z
	movlw 8
	addwf temp7seg,W ; W-reg now holds the upper nibble of the 7-seg port data
	movwf outport
	call POP_STACK
	call SEG_VALUES
	movwf temp7seg
	movlw 0x80
	movwf temp
	movlw 0x7
	call PUSH_STACK ; temp counter
DISPLAY_7seg_NIBBLE_LOOP movf temp,W
	andwf temp7seg,W
	btfsc STATUS,Z
	goto DISPLAY_7seg_NIBBLE_LOOP_NEXT
	movlw 0xf8
	andwf outport,F
	call PEEK_STACK
	addwf outport,F
	;call Delay50
DISPLAY_7seg_NIBBLE_LOOP_NEXT call POP_STACK
	addlw 0xff ;; same as w--
	call PUSH_STACK
	bcf STATUS,C
	rrf temp,F
	btfss STATUS,C
	goto DISPLAY_7seg_NIBBLE_LOOP
	call POP_STACK ; temp counter
	return
	
SEG_VALUES movwf pclTemp
	movlw HIGH SEG_VALUES_TABLE
	movwf PCLATH
	movf pclTemp,W
	addlw LOW SEG_VALUES_TABLE
	btfsc STATUS,C
	incf PCLATH,F
	movwf PCL
SEG_VALUES_TABLE retlw b'01111110';0
	retlw b'00000110';1
	retlw b'11011010';2
	retlw b'11001110';3
	retlw b'10100110';4
	retlw b'11101100';5
	retlw b'11111100';6
	retlw b'01000110';7
	retlw b'11111110';8
	retlw b'11100110';9
	retlw b'11110110';A
	retlw b'10111100';B
	retlw b'01111000';C
	retlw b'10011110';D
	retlw b'11111000';E
	retlw b'11110000';F
	retlw 0x1;decimal point

Delay1s call Delay500
	goto Delay500

Delay500 call Delay255
	call Delay100
	call Delay100
	call Delay20
	call Delay20
	goto Delay5

Delay255	movlw	0xff		;delay 255 mS
		goto	d0
Delay100	movlw	d'100'		;delay 100mS
		goto	d0
Delay50		movlw	d'50'		;delay 50mS
		goto	d0
Delay20		movlw	d'20'		;delay 20mS
		goto	d0
Delay5		movlw	.5		;delay 5.000 ms (4 MHz clock)
d0		movwf	count1
d1		movlw	0x63			;delay 1mS
		movwf	counta
		movlw	0x01
		movwf	countb
Delay_0
		decfsz	counta, f
		goto	$+2
		decfsz	countb, f
		goto	Delay_0

		decfsz	count1	,f
		goto	d1
		retlw	0x00

DEBOUNCE_BUTTONS_MAC btn_state,btn_buffer,btn_pressed
GET_BUTTON_STATE movlw 0
	btfsc BTN_PORT,BTN1
	addlw .1
	btfsc BTN_PORT,BTN2
	addlw .2
	btfsc BTN_PORT,BTN3
	addlw .4
	btfsc BTN_PORT,BTN4
	addlw .8
	btfsc BTN_PORT,BTN5
	addlw 0x10
	btfsc BTN_PORT,BTN6
	addlw 0x20
	btfsc BTN_PORT,BTN7
	addlw 0x40
	btfsc BTN_PORT,BTN8
	addlw 0x80
	return


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
