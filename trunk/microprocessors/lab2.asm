HEXTO7: EQU 0FF1H
DISPV:  EQU 1F12H
KBRD:   EQU 0FEBH

        XOR A
        LD B,A ; register
        LD C,A
        LD D,A
        LD E,A
	LD H,A
        LD L,A

        LD A,0FFH ; immediate
        LD (v1),A

        AND 55H
        LD (v2),A

	LD HL,(v1)
	XOR (HL) ; implied
        LD (v3),A

        LD A,(v2)
        OR 3AH
        LD (v4),A

        LD HL,(v1)
        XOR (HL)
        LD (v5),A
       
        LD A,(v5) ; register indirect
        CPL
        LD (v6),A
       
        LD A,(v1)
	LD HL,v2 ; immediate extended
        XOR (HL) 
	LD HL,v3
        XOR (HL)
	LD HL,v4
        XOR (HL)
	LD HL,v5
        XOR (HL)
	LD HL,v6
        XOR (HL)
        LD (v7),A
       
        LD A,(v6)
        RLC A
        RLC A
        RLC A
        RLC A
        LD (v8),A
       
        LD A,(v2)
        SLA A
        SLA A
        SLA A
        LD (v9),A

SHOW:   LD IX,DISPV
        LD A,(v1)
        LD (IX),A
        LD A,(v2)
        LD (IX+1),A ; indexed
        LD A,(v3)
        LD (IX+2),A
        CALL HEXTO7
        CALL KBRD
       
        LD IX,DISPV
        LD A,(v4)
        LD (IX),A
        LD A,(v5)
        LD (IX+1),A
        LD A,(v6)
        LD (IX+2),A
        CALL HEXTO7
        CALL KBRD
       
        LD IX,DISPV
        LD A,(v7)
        LD (IX),A
        LD A,(v8)
        LD (IX+1),A
        LD A,(v9)
        LD (IX+2),A
        CALL HEXTO7
        CALL KBRD
       
        JP 0000H ; extended
       
v1:     defs 1
v2:     defs 1
v3:     defs 1
v4:     defs 1
v5:     defs 1
v6:     defs 1
v7:     defs 1
v8:     defs 1
v9:     defs 1
