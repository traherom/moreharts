; ---------------------
; Data
; ---------------------
section .data
	check_str: db 'checking',10,0
	infect_str: db 'infecting',10,0
	
	sleep_struct: dw 10000000
				  dw 0
	
	; Storage for vulnerability check
	;check_func: db 0
	
	; Storage for changable infection code
	;infect_func: db 0

; ---------------
; ----- Code ----
; ---------------
section .text
	global _start

; ---------------------
; Entry point
; ---------------------
_start:
	; We're not important... (lower priority)
	mov eax, 34
	mov ebx, 1
	int 80h
	
	main_loop:
	; Make up an IP to connect to
	; TBD
	mov ebx, 10
	
	; Have module check if system is vulnerable
	push check_str
	call print
	add esp, 4
	
	push ebx
	call check_func
	add esp, 4
	
	; Should we try to infect?
	cmp eax, 1
	je wait_loop
	cmp eax, 3
	je wait_loop
	
	; Infect!
	push infect_str
	call print
	add esp, 4
	
	push ebx
	call infect_func
	add esp, 4
	
	; How'd it go?
	; Don't care right now. Later we'll add them to lists as appropriate

	wait_loop:
	; Low and slow (nanosleep)
	mov eax, 4
	mov ebx, [sleep_struct]
	mov ecx, 0 ; Don't care about a run
	int 80h
	
	; Try next guy!
	jp main_loop

; ----------------------------
; Print string to standard out
; ARGS:
; 	1. string memory location
; ----------------------------
print:
	push ebp
    mov  ebp, esp
    
	; Find string end (\0)
	mov edx, [ebp+8]
	dec edx
	
	str_end_search:
	inc edx
	cmp [edx], byte 0
	jne str_end_search
	
	; Calc string length
	sub edx, [ebp+8]
	
	; Write out
	mov eax, 4
	mov ebx, 1
	mov ecx, [ebp+8]
	; EDX already established
	int 80h
	
	; Finish
	pop ebp
	ret

; --------------------------------------------------
; Function to infect given IP
; ARGS:
;	1. Pointer to socket param structure (only IP set)
;
; RETURN:
;	eax - 0 Infected
;		- 1 Infection failed (permanent failure)
;		- 2 Infection failed (temporary failure)
; --------------------------------------------------
infect_func:
	push ebp
    mov  ebp, esp
    
    mov eax, 0
    
	; Finish
	pop ebp
	ret

; --------------------------------------------------
; Function to check if the given IP can be infected
; ARGS:
;	1. Pointer to socket param structure (only IP set)
;
; RETURN:
;	eax - 0 Infectable
;		- 1 Immune
;		- 2 Unknown, run infection routine
;		- 3 Host unreachable (try later)
; --------------------------------------------------
check_func:
	push ebp
    mov  ebp, esp

    mov eax, 1

	pop ebp
	ret
	
