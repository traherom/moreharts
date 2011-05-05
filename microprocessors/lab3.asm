HEXTO7:	EQU 0FF1H
DISPV:	EQU 1F12H
KBRD:	EQU 0FEBH

MAIN:	ORG 1800H

		LD IX, INV1		; inv1 + inv2
		LD IY, INV2		; Immediate extended
		LD HL, ADDOUT
		LD A, (IX)		; Register indirect
		ADD A, (IY)
		LD (HL), A
		INC HL			; next byte
		LD A, (IX+1)	; Indexed
		ADC A, (IY+1)
		LD (HL), A
		INC HL
		LD A, (IX+2)
		ADC A, (IY+2)
		LD (HL), A

		LD IX, INV1		; inv1 - inv2
		LD IY, INV2
		LD HL, SUBOUT
		LD A, (IX)
		SUB (IY)		; Implied
		LD (HL), A
		INC HL			; next byte
		LD A, (IX+1)
		SBC A, (IY+1)
		LD (HL), A
		INC HL
		LD A, (IX+2)
		SBC A, (IY+2)
		LD (HL), A

		LD IX, INV1		; inv1 && inv2
		LD IY, INV2
		LD HL, ANDOUT
		LD A, (IX)
		AND (IY)
		LD (HL), A
		INC HL
		LD A, (IX+01H)
		AND (IY+01H)
		LD (HL), A
		INC HL
		LD A, (IX+02H)
		AND (IY+02H)
		LD (HL), A

		LD IX, INV1		; inv1 || inv2
		LD IY, INV2
		LD HL, OROUT
		LD A, (IX)
		OR (IY)
		LD (HL), A
		INC HL
		LD A, (IX+01H)
		OR (IY+01H)
		LD (HL), A
		INC HL
		LD A, (IX+02H)
		OR (IY+02H)
		LD (HL), A

		LD IX, INV1		; inv1 XOR inv2
		LD IY, INV2
		LD HL, XOROUT
		LD A, (IX)
		XOR (IY)
		LD (HL), A
		INC HL
		LD A, (IX+01H)
		XOR (IY+01H)
		LD (HL), A
		INC HL
		LD A, (IX+02H)
		XOR (IY+02H)
		LD (HL), A

DISP:	LD IX, DISPV	; show each originals and result

		LD IY, INV1
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7		; Extended
		CALL KBRD

		LD IY, INV2
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD

		LD IY, ADDOUT
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD

		LD IY, SUBOUT
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD

		LD IY, ANDOUT
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD

		LD IY, OROUT
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD

		LD IY, XOROUT
		LD A, (IY+2)
		LD (IX), A
		LD A, (IY+1)
		LD (IX+1), A
		LD A, (IY)
		LD (IX+2), A
		CALL HEXTO7
		CALL KBRD
        
END:    HALT

VARS	org 1D00H
INV1	defs 3
INV2	defs 3
ADDOUT	defs 3
SUBOUT	defs 3
ANDOUT	defs 3
OROUT	defs 3
XOROUT	defs 3
