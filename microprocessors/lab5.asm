;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dear Colegue,                                                         ;
; This  is the beginning of my calculator program.  The main keyboard   ;
; input routine is written, it will fetch two 3-nibble values from the  ;
; user via the keypad and store them into memory.  It then calls the    ;
; keypad one more time to get the operator (reverse polar notation).    ;
; This is where I was when I took my vocation.  You need to finish 	;
; writing the logic that will decide which function to   		;
; call based on the key that was pressed for the function.  Here are    ;
; the function mappings given by management:                            ;
;                                                                       ;
; Key     KBRD-Value    Function                                        ;
; NEXT       10H        Add U1 to U2                                    ;
; PREV       11H        Subtract U2 from U1                             ;
; GO         12H        Multiply U1 x U2                                ;
; any other             Sound tone, prompt for new operation key        ;
;                                                                       ;
; After displaying the result, you must call KBRD to determine if you   ;
; should run the program again.  If you receive the STEP key from the   ;
; keyboard after displaying the result, you should terminate the program;
; any other key at this point should rerun the program.                 ;
; The STEP key returns the value 13H                                    ;
;                                                                       ;
; Our supervisor has already done the software review on the functions  ;
; I have written and they have passed QA.  Therefore, you are not       ;
; permitted to make any changes to the existing code.  Your changes     ;
; must appear in the areas designated.                                  ;
;                                                                       ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

KBRD	EQU	0FEBH		; The system equates defined by ZAD
HEXTO7	EQU	0FF1H
MESOUT	EQU	0FF4H
TONE	EQU	0FFAH
DISPV	EQU	1F12H

	ORG	1800H
	JP	MAIN		; Jump over my functions and get to main
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                DO NOT MODIFY ANY EXISTING CODE                    ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GETVAL: Function to fetch a 12-bit number from the user
; ENTRY:  User must provide in the IY a pointer to a 2-byte location
;         where they want the 12-bit number stored (little indian)
; EXIT:   GETVAL will obtain a 12-bit (3 nibble) value from the user
;         in HEX format and store it at the location pointed to by the
;         IY register.  All registers are returned un-changed
; NOTE:   Calls function GETNIB which actually gets one nibble 
;         at a time
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GETVAL:
	PUSH	AF		; Save all registers
	PUSH	BC
	PUSH	DE
	PUSH	HL
	PUSH	IX
	LD	IX,DISPV	
	PUSH	IY
	LD	A,0
	LD	(IX),A
	LD	(IX+1),A
	LD	(IX+2),A	    	         
	LD	B,0		; Ask GETNIB for 1st nibble 
	CALL	GETNIB
	LD 	B,1		; Ask GETNIB for 2nd nibble
	CALL	GETNIB		
	LD 	B,2		; Ask GETNIB for 3rd nibble
	CALL	GETNIB		 
	LD	HL,40D		; Sound tone indicating value was received
	CALL	TONE		
	LD	A,(DISPV+2)	; Copy 12-bit value from storage in DISPV to
	POP	IY		;    the variable passed in by pointer
	LD	(IY),A
	LD	A,(DISPV+1)	
	LD	(IY+1),A
	CALL	HEXTO7		; Display the final value so the user can see
	CALL	KBRD		;   it before proceeding
	POP	IX		; Restore the registers to original state
	POP	HL
	POP	DE
	POP	BC
	POP	AF
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GETNIB:  This is an internal function called by GETVAL to fetch a 1-digit
;          HEX nibble.  It checks for valid values accepting only 0-F keys.
;          This function assembles a 12-bit (3-nibble) value and stores it in 
;          the DIPSV variable.  The calling function (GETVAL) must copy the 
;          final value from DISPV.  
; ENTRY:   The calling function must specify in the B register which nibble is 
;          being fetched.  0=first value, 1=second, 2=third.  The 3-nibble value
;          is assembled by assuming the calls are made in sequential order.  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GETNIB:
	CALL	HEXTO7		;Display current value and get next digit
	CALL	KBRD
	CP	10H		;Check key press to verify between 0-F
	JP	M,NIBOK
	LD	HL,250D
	CALL	TONE
	JP	GETNIB		;If not valid digit, sound tone and re-enter digit
NIBOK:
	LD	C,A		;Hold the value aside
	LD	A,B		;Which nibble is this supposed to be?
	CP	0		
	JP	NZ,NOT0		;If 1st nibble then
	LD	(IX+2),C	;   save as first nibble
	RET
NOT0:	CP	1		;Else If 2nd Nibble Then
	JP	NZ,NOT1
	LD	A,(IX+2)	;   Get 1st nibble from memory
	RLC	A		;   Shift it over to left to make room for 2nd
	RLC	A
	RLC	A
	RLC	A
	OR	C		;   Append 2nd nibble behind first
	LD	(IX+2),A	;   Save 1st and 2nd nibble as a byte
	RET
NOT1:	LD	A,(IX+2)	;Else If 3rd Nibble Then
	SRL	A		;   Get first byte and pull out 1st nibble
	SRL	A
	SRL	A
	SRL	A		;    
	LD	(IX+1),A	;   Save 1st nibble as New Byte  (the MSB)
	LD	A,(IX+2)	;   Get first byte again
	SLA	A		;   Shift out the first nibble making room for 3rd
	SLA	A
	SLA	A
	SLA	A		
	OR	C		;   Stick 3rd nibble into second making new LSB
	LD	(IX+2),A	;   Save the LSB
	RET

;;;;;;;;;;;;;;; END OF MY FUNCTIONS ;;;;;;;;;;;;;;;;;;

MAIN:
	LD	IX,MSG1		; Prompt user for the first 12-bit value
	CALL	MESOUT
	CALL	KBRD
	LD	IY,U1		; Point to where the 12-bits should be stored
	CALL	GETVAL		; Make call for 12-bit input

	LD	IX,MSG2		; Prompt user for the second value
	CALL	MESOUT
	CALL	KBRD
	LD	IY,U2		; Point to storoage for 2nd value
	CALL	GETVAL		; Call for the value

PROMPT:	LD	IX,MSG3		; Prompt user for Operation
	CALL	MESOUT
	CALL	KBRD		; A reg has the function code
	
	LD	IY, U1		; Get paramaters ready for function call
	LD	A, (IY+0)
	LD	H, A
	LD	A, (IY+1)
	LD	L, A		; is the order of bytes correctly loaded?
	LD	IY, U2
	LD	A, (IY+0)
	LD	D, A
	LD	A, (IY+1)
	LD	E, A
	
NEXTK:	CP	10H		; NEXT key? - add
	JP	NZ, PREVK
	CALL	ADD
	JP	DISP
PREVK:	CP	11H		; PREV? - subtract
	JP	NZ, GOK
	CALL	SUB
	JP	DISP
GOK:	CP	12H		; GO? - multiply
	JP	NZ, UNK
	CALL	MPY
	JP	DISP
UNK:				; Unrecognized
	JP	PROMPT		; sound tone (need to look up) then return to prompt

DISP:	LD	IX, DISPV	; Shows what's in (in MSB->LSB order)
	LD	(IX+2), D	; HLDE, but only show LDE for now...
	LD	(IX+1), E	; assume H is always 0? Will be for
	LD	(IX+0), L	; add/sub, maybe not mult
	CALL	HEXTO7
	
CONT:	CALL	KBRD		; Should we continue?
	CP	13H		; STEP? - end program
	JP	NZ, MAIN
	END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; All these functions (ADD, SUB, MPY) take two paramaters in         ;
; HL and DE and return the result on the top of the stack as 4 bytes ;
; in those registers, with significance going HLDE                   ;
; When order matters (ie, subtraction), HL comes first, so HL-DE     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; ADDITION ;;;
ADD:
	ADD HL, DE
	LD DE, HL
	LD HL, 00H ; DE is low, HL is high and will always be empty
	RET

;;; SUBTRACTION ;;;	
SUB:
	LD A, H	; Do each byte separately
	SUB D
	LD H, A
	LD A, L
	SBC E
	
	BIT 3, A	; Because technically our inputs are 12-bit, we need to check to
	JP Z, SNONEG ; see if that 12th bit became 1, IE we have a negative.
	OR 0F0H	; If it did, then carry the sign to the end
	
SNONEG:	LD L, A
	LD DE, HL
	LD HL, 00H ; See ADD
	RET
	
;;; MULTIPLICATION ;;;
MPY:
	LD A, H
	OR L
	JP Z, MZERO
	LD A, D
	OR E
	JP Z, MZERO
	LD C, 16
	LD XI, MULTIPLIER
	LD IY, MRESULT
	LD A, 0
	LD (IY+0), A
	LD (IY+1), A
	LD (IY+2), A
	LD (IY+3), A
	LD (MULTIPLIER), DE
	LD (IX+2), A
	LD (IX+3), A
MTOP:	LD A, H
	OR L
	JP Z, MPDONE
	RR H
	RR L
	JP NC, NOADD
	LD A, (IY)
	ADD A, (IX)
	LD A, (IY+1)
	ADC A, (IX+1)
	LD (IY+1), A
	LD A, (IY+2)
	ADC A, (IX+2)
	LD (IY+2), A
	LD A, (IY+3)
	ADC A, (IX+3)
	LD (IY+3), A
NOADD:	SLA (IX)
	RL (IX+1)
	RL (IX+2)
	RL (IX+3)
	DEC C
	JP NZ, MTOP
MPDONE:	LD DE, (MRESULT)
	LD HL, (MRESULT+2)
	RET
MZERO:	LD DE, 00H
	LD HL, 00H
	RET
	
MULTIPLIER:	DEFS 4
MRESULT: DEFS 4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  VARIABLE STORAGE SPACE:                                         ;
;  You may add to these variables, however, DO NOT change the      ;
;  variables that are already created.  The existing code needs    ;
;  these variables to be defined as they are.                      ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MSG1:	DEFM	'U1    '	; Prompt Message Constants
MSG2:	DEFM	'U2    '
MSG3:	DEFM	'OPCODE'

U1:	DEFS	2		; Variable Space
U2:	DEFS	2

