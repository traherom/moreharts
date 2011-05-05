;---------------------- Lab Report ----------------------------
; Lab by: Ryan Morehart, Adam Oakley
;
; The software is pretty staight forward. After setup, it drops
; into MAIN, which loops infinitly. MAIN waits for the proper
; amount of time, then grabs values from the ADC. If averaging
; is on, it keeps feeding AVG values until it indicates it has
; gotten enough. It then displays the 8-bit value which has
; been saved in the memory location at VOLTAGE either as hex
; by calling PRINTX or as a decimal voltage (to both the PC
; and the BCD display) by calling PRINTV.
;
; The voltage passed in to these display functions is just a 
; raw value from 0-255. This number is coverted to a BCD value
; by multiplying by 5. This leaves the first digit in the high
; order byte. We pass this byte off to PRINTN, which adds '0'
; to any value less than 10 and 'A' for any value greater. This
; results in any hex value 0-15 being properly converted to the
; corresponding ASCII value. To get the remaining digits from
; the BCD value, the following cycle is repeated 3 times:
;	1. Clear high order byte
;	2. Multiply by 10
;	3. Display high order
;
; The entire time this main loop is going on, a user keypress
; on the computer may generate an interrupt. This key is
; processed by the ISR to either display the developer's names
; or change a setting of the program, including whether to
; display using hex or decimal, sample 1 or 2 times a second,
; or turn averaging on or off. Since it's in interrupt mode 2,
; The ISR is found by the Z80 by taking the value in the
; interrupt register (I) as the high order byte and the value
; sent by the SIO (FE, since this is a receive).
;
; With averaging on, the voltages displayed are more stable,
; not fluctuating nearly as frequently. However, they do still
; tend to vary by a small but noticable amount, so the benefit
; may not be as significant as might be expected.
;
; The voltmeter works down to millivolts. However, the
; amount of flucuation in the display at that bottom digit is
; significant enough it might be considered unreliable. Giving
; it the benefit of the doubt though, instability in the
; potentiometer might play a part in this.
;
;      +-------+----------------+--------+
;      |PC (V) | Multimeter (V) | % Error|
;      +-------+----------------+--------+
;      | 0.546 |     0.558      |  2.198 |
;      | 1.113 |     1.068      | -4.043 |
;      | 1.542 |     1.503      | -2.529 |
;      | 2.089 |     2.041      | -2.298 |
;      | 2.558 |     2.492      | -2.580 |
;      | 3.015 |     3.021      |  0.199 |
;      | 3.574 |     3.496      | -2.182 |
;      | 4.121 |     4.017      | -2.524 |
;      | 4.609 |     4.497      | -2.430 |
;      +-------+----------------+--------+
;         Absolute average error: 2.331%
;
; As the table shows, our program/hardware is within 2% of
; actual voltage on average (with no major outliers).
;--------------------------------------------------------------

	; Hardware addresses
ADC	EQU 0C8H	; ADC INPUT
ADCI	EQU 0C0H	; ADC address for checking ready state
BCD	EQU 0C0H	; BCD OUTPUT
ADATA	EQU 0D0H	; Channel A on SIO's data address
ACMD	EQU 0D2H	; Control address for channel A on the SIO
BCMD	EQU 0D3H	; Control address for channel B on the SIO

	; ZAD functions
DELAY	EQU 0FFDH

;----------------------------------------------------------
; Setup function, initializes settings, serial communication,
; and interrupts
	ORG 1800H
SETUP:	DI	; Don't let us be interrupted until done setting up

	; SIO
	LD A, 18H
	OUT (ACMD), A
	
	LD A, 02H
	OUT (BCMD), A
	LD A, 0FEH
	OUT (BCMD), A
	
	LD A, 01H
	OUT (BCMD), A
	LD A, 00H
	OUT (BCMD), A

	LD A, 04H
	OUT (ACMD), A
	LD A, 04H
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
	
	; Register ISR
	LD A, 19H
	LD I, A
	LD HL, 1A00H
	LD (19FEH), HL
	IM 2
	
	; Set default settings
	; SETS is a series of 8 flags:
	; Bit 0 - .5/1 second delay
	;     1 - averaging on?
	;     2 - decimal voltage/raw hex
	;     3 - unused
	;     4 - unused
	;     5 - unused
	;     6 - unused
	;     7 - average ready?
	; Default: 1 sec delay, averaging off, decimal display
	LD A, 01H
	LD (SETS), A
	
	; Welcome to system, we're ready to go
	LD HL, MSRDY
	CALL PRINTS
	
	; Start system
	EI

;----------------------------------------------------------
; Main controller, handles display based on settings
MAIN:	; Loop continuously reading/displaying the voltage as given in flags
	; How long is our delay?
	LD HL, SETS
	BIT 0, (HL)
	JP NZ, DHALFS ; Twice a second, only delay for 30 seconds
	LD HL, 500D
	CALL DELAY
DHALFS:	LD HL, 500D
	CALL DELAY

	; Get ADC value to A
GETADC:	CALL RDADC

	; Are we averaging or just taking one at a time?
	LD HL, SETS
	BIT 1, (HL)
	JP Z, DIRECT
	
	; Get average and see if we're done with that
	CALL AVG
	
	LD HL, SETS
	BIT 7, (HL)
	JP Z, GETADC ; Not done yet, go back and get another value
	RES 7, (HL) ; Reset "average ready" flag for next time
	JP DISP

DIRECT:	LD (VOLTAGE), A
	
DISP:	; How should we display (decimal volt or hex)?
	BIT 2, (HL)
	JP NZ, DHEX
	
	; Display as decimal voltage
	LD A, (VOLTAGE)
	CALL PRINTV
	JP MAIN
	
DHEX:	; Display as hex
	LD A, (VOLTAGE)
	CALL PRINTX
	JP MAIN

;----------------------------------------------------------
; Functions for keeping average
RESAVG:	; Clear average for first run
	PUSH AF
	PUSH HL
	
	LD A, 8
	LD (AVGCNT), A
	LD HL, 00H
	LD (AVGTOT), HL
	
	POP HL
	POP AF
	RET

AVG:	; Accumulates values until enough are in, then displays and resets avg
	; Add to total
	PUSH BC
	PUSH HL
	
	; Add new value on
	LD B, 0
	LD C, A
	LD HL, (AVGTOT)
	ADD HL, BC
	LD (AVGTOT), HL
	
	; If we have needed samples save off otherwise return to get more
	LD HL, AVGCNT
	DEC (HL)
	JP Z, FNAVG
	
	POP HL
	POP BC
	RET

FNAVG:	; take average (total/8)
	LD HL, (AVGTOT)
	SRL H
	RR L
	SRL H
	RR L
	SRL H
	RR L
	LD A, L
	LD (VOLTAGE), A
	
	; Reset average to keep going
	CALL RESAVG
	
	; Note we're ready for display
	LD HL, SETS
	SET 7, (HL)
	
	POP HL
	POP BC
	RET
	
;----------------------------------------------------------
; Display functions
PRINT:	; Sends one charater in A to PC
	PUSH AF
PLOOP:	IN A, (ACMD) ; busy loop until SIO is ready to send
	BIT 2, A     ; next char. Otherwise would overwrite
	JP Z, PLOOP  ; one already waiting
	POP AF
	OUT (ADATA), A
	RET

PRINTN: ; Print a single hex digit converted to hex (0-15)
	; If given a "decimal" number it will still go through correctly,
	; since it's equivalent in hex anyway
	PUSH AF
	CP 10d
	JP NC, PRTLT
	ADD A, '0' ; Show as number
	CALL PRINT
	POP AF
	RET
PRTLT:	SUB 0AH
	ADD A, 'A' ; Otherwise show as letter
	CALL PRINT
	POP AF
	RET
	
PRINTV:	; Print voltage in A to both PC and the BCD display
	PUSH AF
	PUSH BC
	PUSH DE
	PUSH HL
	
	; Covert to voltage: (A/256)x5
	; Multiply by 5
	LD H, 00H
	LD L, A
	LD B, H
	LD C, L
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC

	; Save one's digit for BCD display in the future
	LD D, H
	SLA D
	SLA D
	SLA D
	SLA D
	
	; Show one's digit
	LD A, H
	CALL PRINTN
	LD A, '.'
	CALL PRINT

	; Get and show next digit by multiplying by 10
	; Tenths
	LD H, 00H
	LD B, H
	LD C, L
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	
	LD A, H
	CALL PRINTN

	; Add this digit and show on board BCD display
	LD A, D
	OR H
	OUT (BCD), A

	; Hundreds
	LD H, 00H
	LD B, H
	LD C, L
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	
	LD A, H
	CALL PRINTN
	
	; Thousandths (millivolts)
	LD H, 00H
	LD B, H
	LD C, L
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	ADD HL, BC
	
	LD A, H
	CALL PRINTN
	
	; Note that this is a voltage
	LD A, ' '
	CALL PRINT
	LD A, 'V'
	CALL PRINT
	CALL ENDL
	
	; Restore registers and finish
	POP HL
	POP DE
	POP BC
	POP AF
	RET

PRINTX:	; Prints a raw 8-bit value as hex to the PC
	PUSH AF
	PUSH HL
	
	LD L, A
	
	; 'hex prefix,' if you will
	LD A, '0'
	CALL PRINT
	LD A, 'x'
	CALL PRINT

	; Display 4 MSBits	
	LD A, L
	SRL A
	SRL A
	SRL A
	SRL A
	CALL PRINTN
	
	; Display 4 LSBs
	LD A, L
	AND 0FH
	CALL PRINTN
	CALL ENDL
	
	POP HL
	POP AF
	RET

ENDL:	; End line with \r\n
	PUSH AF
	LD A, 0DH
	CALL PRINT
	LD A, 0AH
	CALL PRINT
	POP AF
	RET

PRINTS:	; Sends a null-terminated string starting at the address in HL
	PUSH AF
	PUSH HL
PSLP:	LD A, (HL)
	CP 0
	JP Z, PRNTSE
	CALL PRINT
	INC HL
	JP PSLP
PRNTSE:	CALL ENDL
	POP HL
	POP AF
	RET

;----------------------------------------------------------
; Read functions
RDPC:	; Gets one character from the PC into A
	IN A, (ADATA)
	RET

RDADC:	; Get value from ADC
	; Turn on ADC
	OUT (ADC), A
ADCLP:	; Busy wait for it to be ready
	IN A, (ADCI)
	BIT 0, A
	JP NZ, ADCLP
	
	; Read in value
	IN A, (ADC) 
	RET

;----------------------------------------------------------
; Key press handler
	ORG 1A00H
ISR:	; Gets the key pressed and changes settings as needed
	DI
	PUSH AF
	PUSH BC
	PUSH DE
	PUSH HL

	; What key was pressed?
	CALL RDPC
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
	LD HL, MSKYERR
	CALL PRINTS
	JP ISRE

K1:	LD HL, MSSM1
	CALL PRINTS
	LD HL, SETS
	RES 0, (HL)
	JP ISRE
K2:	LD HL, MSSM2
	CALL PRINTS
	LD HL, SETS
	SET 0, (HL)
	JP ISRE
KA:	LD HL, MSAVG1
	CALL PRINTS
	LD HL, SETS
	SET 1, (HL)
	CALL RESAVG	; Clear average so it begins fresh
	JP ISRE
KLA:	LD HL, MSAVG0
	CALL PRINTS
	LD HL, SETS
	RES 1, (HL)
	JP ISRE
KX:	LD HL, MSHEX1
	CALL PRINTS
	LD HL, SETS
	SET 2, (HL)
	JP ISRE
KLX:	LD HL, MSHEX0
	CALL PRINTS
	LD HL, SETS
	RES 2, (HL)
	JP ISRE
KD:	LD HL, MSDEVS
	CALL PRINTS
	JP ISRE

ISRE:	; Restore registers and finish up here
	POP HL
	POP DE
	POP BC
	POP AF
	
	EI
	RETI

;-----------------------------------------------------------
; Strings ready to send to PC using PRINTS. Names should all
; begin with 'MS' designate a message and be null (\0) terminated
MSRDY	DEFM 'Initialized'
	DEFB 0
MSKYERR	DEFM 'Invalid key pressed'
	DEFB 0
MSDEVS	DEFM 'Ryan Morehart and Adam Oakley'
	DEFB 0
MSHERE	DEFM 'Here'
	DEFB 0
MSSM1	DEFM 'Sample rate 1/sec'
	DEFB 0
MSSM2	DEFM 'Sample rate 2/sec'
	DEFB 0
MSAVG1	DEFM 'Averaging on'
	DEFB 0
MSAVG0	DEFM 'Averaging off'
	DEFB 0
MSHEX1	DEFM 'Hex display on'
	DEFB 0
MSHEX0	DEFM 'Decimal voltage display on'
	DEFB 0

;-----------------------------------------------------------
; Storage
; See initialization for description of each bit
SETS	DEFS 1 ; Settings

AVGCNT	DEFS 1 ; Number of samples to go for average
AVGTOT	DEFS 2 ; Averaging total storage

VOLTAGE	DEFS 1 ; 8-bit voltage for display

