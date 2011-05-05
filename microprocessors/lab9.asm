ADC	EQU 1C08H	; Er... what's the address?
BCD	EQU 1C00H	; same... address?
ZADISR	EQU 1F41H	; ZAD looks here to see where to call the ISR at

	ORG 1800H
	DI		; Don't let us be interrupted until
	IM 1		; the mode is set correctly
	
	LD HL, ISR	; Put address of ISR into 1F41/1F42H (low/high respectively)
	LD (ZADISR), HL
	
MAIN:	OUT (ADC), A
	EI
	HALT
	JP MAIN

ISR:	LD B, 00H	; Read in next value
	IN C, (ADC)
	ADD HL, BC
	
	DEC D		; Do we have 8 samples now?
	JP Z, AVG
	RETI		; End ISR, still accumulating

AVG:	SRA H		; Divide HL (ADC raw total) by 8 to get average
	RR L
	SRA H
	RR L
	SRA H
	RR L

	LD D, 00H	; Convert to decimal 0.0-5.0 voltage: (ADC/256)*5
	LD E, 5
	CALL MPY
	LD B, D		; Save one's digit
	
	LD H, 00H	; Multiply by 10 to get tens digit,
	LD L, 10	; which will end up in the high byte of the result
	CALL MPY
	LD C, D
	
	LD A, B		; Combine bytes to one BCD value
	SLA A
	SLA A
	SLA A
	SLA A
	OR C
	
	OUT (BCD), C	; Display
	
	LD HL, 00H	; Clear out average to begin again
	LD D, 8
	RETI		; All done with ISR for now

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
