HEXTO7:	EQU 0FF1H
DISPV:	EQU 1F12H
KBRD:	EQU 0FEBH

START:	LD HL, 0
	CALL SHOW

	CALL KBRD	; MSB nibble
	SLA A		; Move to right place in btye
	SLA A
	SLA A
	SLA A
	LD L, A		; Save first
	CALL SHOW	; Show
	
	CALL KBRD	; LSB nibble
	OR L		; Combine two nibbles
	LD L, A
	CALL SHOW	; Show whole thing
	
	;CALL KBRD	; Wait for user to tell us to go on

	CP 0		; SHOW might have changed CY
	JP Z, END	; Stop if 0 entered

	LD B, L		; Save max number
	BIT 0, B	; Change to odd number if needed
	JP NZ, ISODD
	DEC B
	
ISODD:	LD HL, 0	; Initialize loop
	LD DE, 1
	
LOOP:	ADD HL, DE	; Add odd to total

	LD A, E		; Stop if we reached the sum
	CP B
	JP Z, DONE
	
	INC DE		; Next odd number
	INC DE
	JP LOOP

DONE:	CALL SHOW	; show total
	CALL KBRD	; Wait for keypress
	
	JP START	; Go get another number to add to

END:	HALT

SHOW:	LD IX, DISPV	; Show what's been pressed
	LD (IX+2), L	; LSB
	LD (IX+1), H	; MSB
	LD (IX), 00H	; Make sure it's blank here
	CALL HEXTO7
	RET
	
