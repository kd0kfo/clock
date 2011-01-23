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
	;more interrupt stuff can go here
END_OF_INTERRUPT nop
	retfie
	;
	
INIT 
	INIT_STACK_MAC stackHead,stackPtr
	movlw 0x3
	movwf low buffer_7seg + RIGHT_DISPLAY
	bcf STATUS,RP0
	bcf STATUS,RP1;
	clrf controlPort
	clrf dipControl
	clrf outport
	bsf STATUS,RP0;bank 1
	movlw 0x0; all output
	movwf outportTRIS; Hours/~Minutes(7bit), Binary/~7-seg(6bit), When 7-seg is on bit 4 is ~minutes and bit 7 is ~hour
	movlw 0x6;
	movwf ADCON1;SET bit 0 of porta to be digital (not analog :-/ )
	movlw b'00010001';TIMER MUST BE INPUT!!!
	movwf controlPortTRIS;bit 0 = hex/~octal, bit 1 = external alarm trigger, 4 = timer0 clock
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
    ;movf resetTMR0,W
	;movwf TMR0; Count from c4 to overflow (60secs) then interrupt
	call BANK_0
	call DISPLAY_7seg
MAIN_LOOP clrwdt
	goto MAIN_LOOP

;bin 7-seg display macros
;CREATE_DISPLAY_MAC myStatus,minutes,hours,MAKE_PACKET,binaryMinute,binaryHours,controlPort,rightDisplay,SEG_VALUES,output,LOAD_PACKET,indicator,hexToOctal

BANK_0 bcf STATUS,RP0
	bcf STATUS,RP1
	return

	;
ERROR_RETURN nop;not sure what to do in case of error yet
	return
	;
	;macro calls
STACK_MAC stackTemp,stackTemp2,stackPtr

DISPLAY_7seg movf buffer_7seg + RIGHT_DISPLAY,W
	andlw 0xf
	call PUSH_STACK ; value to display
	movlw RIGHT_DISPLAY ; left digit
	call PUSH_STACK
	movlw RIGHT_DISPLAY ; left 7seg
	call PUSH_STACK
	call DISPLAY_7seg_NIBBLE
	return	

DISPLAY_7seg_NIBBLE clrf temp7seg
	call POP_STACK
	xorlw LEFT_DISPLAY	
	btfss STATUS,Z
	bsf temp7seg,7
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
	movlw 0xf0
	andwf outport,F
	call POP_STACK
	addwf outport,F
	addlw 0xff ;; same as w--
	call PUSH_STACK
	clrwdt
	call Delay50
DISPLAY_7seg_NIBBLE_LOOP_NEXT bcf STATUS,C
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
