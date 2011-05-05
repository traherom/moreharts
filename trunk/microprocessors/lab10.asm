	; Hardware addresses
ADC	EQU 0C8H	; ADC INPUT
BCD	EQU 0C0H	; BCD OUTPUT
ADATA	EQU 0D0H	; Channel A on SIO's data address
ACMD	EQU 0D2H	; Control address for channel A on the SIO
BCMD	EQU 0D3H	; Control address for channel B on the SIO

	; Functions
DELAY	EQU ???

	ORG 1800H
SETUP:	DI		; Don't let us be interrupted until done setting up
	
	; interrupt register
	LD A, 19H
	LD I, A
	
	; SIO
	LD A, 18H
	OUT (ACMD), A
	
	LD A, 02H
	OUT (BCMD), A
	LD A, 0FEH
	OUT (BCMD), A
	
	LD A, 04H
	OUT (ACMD), A
	LD A, 0C1H
	OUT (ACMD), A
	
	LD A, 03H
	OUT (ACMD), A
	LD A, 0C1H
	OUT (ACMD), A
	
	LD A, 05H
	OUT (ACMD), A
	LD A, 068H
	OUT (ACMD), A
	
	LD A, 01H
	OUT (ACMD), A
	LD A, 18H
	OUT (ACMD), A
	
	; register ISR
	LD HL, ISR ; get the address of the ISR dynamically... why do people do it fixed?
	LD (19FEH), HL
	IM 2
	EI
	
	; Set default settings
	LD A, 0
	LD (SETS), A
	
	; Welcome to system, we're ready to go (plus a nice test... are things working?
	LD HL, MSRDY
	CALL PRINTS
	
MAIN:	; Loop continuously reading/displaying the voltage as given in flags
	LD HL, SETS

	; How long is our delay?
	BIT 0, (HL)
	JP NZ, DHALFS ; Twice a second, only delay for 30 seconds
	LD A, 3000
	CALL DELAY ; UM... how do you specify delay? I tried in A
DHALFS:	LD A, 3000
	CALL DELAY
	
	; Get value to BC
	LD B, 00H
	IN A, (ADC)
	LD C, A
	
	; Are we averaging?
	BIT 1, (HL)
	JP Z, SDRECT
	CALL AVG
	JP DISP
SDRECT:	; No average, mark for display immediately
	LD HL, SETS
	SET 7, (HL)
	
DISP:	; Only display if we're marked to
	LD HL, SETS
	BIT 7, (HL)
	JP Z, MAIN
	
	; How should we display (decimal volt or hex)?
	BIT 3, (HL)
	JP NZ, DHEX
	LD HL, BC
	CALL PRINTV
	JP MAIN
	
DHEX:	; Display as hex
	LD HL, BC
	CALL PRINTX
	JP MAIN

;----------------------------------------------------------
; Functions for keeping/displaying average
RESAVG:	; Clear average for first run
	LD A, 5
	LD (AVGCNT), A
	LD A, 0
	LD (AVGTOT), A
	RET

AVG:	; Accumulates values until 5 are in, then displays and resets avg
	; Add to total
	LD HL, (AVGTOT)
	ADD HL, BC
	LD (AVGTOT), HL
	
	; If we have needed samples display otherwise return to get more
	LD HL, AVGCNT
	DEC (HL)
	JP Z, FNAVG
	RET

FNAVG:	; Note we're ready to display
	LD BC, (AVGTOT)
	LD HL, SETS
	SET 7, (HL)
	
	; Reset average to keep going	
	CALL RESAVG
	RET
	
;----------------------------------------------------------
; Display functions
PRINT:	; Sends one charater in A to PC
	OUT (ADATA), A
	RET

PRINTN:	; Print a number (converts to ascii then sends)
	ADD A, 30H
	CALL PRINT
	RET

PRINTV:	; Print voltage in HL to both PC and the BCD display
	LD B, H	; Save one's digit

	LD D, 00H
	LD E, L

	LD H, 00H	; Multiply by 10
	SLA L
	RL H
	SLA L
	RL H
	SLA L
	RL H
	ADD HL, DE
	ADD HL, DE
	LD C, H

	LD A, B		; Combine bytes to one BCD value
	SLA A
	SLA A
	SLA A
	SLA A
	OR C
	
	OUT (BCD), A	; Display to BCD
	
	; NEED TO SHOW TO PC!
	LD A, ' '
	CALL PRINT
	LD A, 'V'
	CALL PRINT
	CALL ENDL
	
	RET

PRINTX:	; Prints a voltage as hex to the PC
	LD A, L ; NOT CORRECT AT ALL! Need to do some shifting and stuff
	CALL PRINTN ; didn't feel like it though at the moment

	LD A, '0'
	CALL PRINT
	LD A, 'x'
	CALL PRINT
	
	CALL ENDL
	RET

ENDL:	; Ends line with \r\n
	LD A, 0DH
	CALL PRINT
	LD A, 0AH
	CALL PRINT
	RET

PRINTS:	; Sends a string \0 terminated at the address in HL
	LD A, (HL)
	JP Z, PRNTSE
	CALL PRINT
	INC HL
	JP PTMSG
PRNTSE:	CALL ENDL
	RET

;----------------------------------------------------------
; Read functions
READ:	; Gets one character from the PC into A
	IN A, (ADATA)
	RET

;----------------------------------------------------------
; Key press handler
ISR:	; Gets the key pressed and changes settings as needed
	DI
	
	; What key was pressed?
	CALL READ
	CP '1'	; Sample rate 1/sec
	JP Z, K1
	CP '2'	; Sample rate 2/sec
	JP Z, K2
	CP 'A'	; Turn on averaging
	JP Z, KA
	CP 'a'	; Turn off averaging
	JP Z, KLA
	CP 'X'	; Turn on hex
	JP Z, KX
	CP 'x'	; Turn off hex
	JP Z, KLX
	CP 'D'	; Display dev's names
	JP Z, KD
	
	; Not found if it reaches here
	;LD HL, SETS
	;SET 4, (HL)
	LD HL, HSKYERR
	CALL PRINTS
	JP ISRE

K1:	LD HL, SETS
	RES 0, (HL)
	JP ISRE
K2:	LD HL, SETS
	SET 0, (HL)
	JP ISRE
KA:	LD HL, SETS
	SET 1, (HL)
	CALL RESAVG	; Clear average so it begins fresh
	JP ISRE
KLA:	LD HL, SETS
	RES 1, (HL)
	JP ISRE
KX:	LD HL, SETS	; Unused for now, bonus credit
	SET 2, (HL)
	JP ISRE
KLX:	LD HL, SETS
	RES 2, (HL)
	JP ISRE
KD:	LD HL, SETS
	RES 3, (HL)
	LD HL, MSDEVS
	CALL PRINTS
	JP ISRE

ISRE:	EI
	RETI

;-----------------------------------------------------------
; Storage
SETS	DEFS 1 ; Settings
AVGCNT	DEFS 1 ; Number of samples to go for avg
AVGTOT	DEFS 2 ; Averaging total storage

;-----------------------------------------------------------
; Strings ready to send to PC using PRINTS. Names should all
; begin with 'MS' designate a message
MSRDY	DEFB "Initialization complete, welcome", 0
MSKYERR	DEFB "Invalid key pressed", 0
MSDEVS	DEFB "Ryan Morehart, Adam Oakley", 0

