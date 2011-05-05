#! /usr/bin/env mzscheme
#lang scheme
(require (lib "pregexp.ss"))

; ------------------------------
; Random stuff
;; string-pad is provided by some Schemes, but I can't seem to find it for PLT
(define (string-pad str len char)
  (if (>= (string-length str) len)
      (substring str 0 len) ; never let it be _longer_ than len either
      (string-pad (string-append char str) len char)))

;; This function is pretty much the most disgusting thing ever, but it works:
;; Takes a number, converts it to an int, then strips off the annoying .0 scheme leaves
(define (int-part float) (string->number (list-ref (pregexp-split "[.]" (number->string (floor float))) 0)))

; ------------------------------
; Emulator utility functions
;; Ensures the memory is 1k long
(define (init-mem data) (if (< (length data) 1000) (init-mem (append data (list "invalid"))) data))

;; Saves the given data as an 11 character number with sign
(define (set-data data pos val)
  (append 
   (take data pos) ; Grab left part of list
   (list ; Make new value into nice string
    (if (string? val)
        val ; Already a string, just put in list
        (string-append 
         ; Need to coerce to a nice string
         (if (<= 0 val) "+" "-")
         (string-pad (number->string (int-part (if (<= 0 val) val (- 0 val)))) 10 "0")
         )
        )
    )
   (drop data (+ pos 1)))) ; Grab right part of list, minus the element we're rplacing

;; Returns the data point as a number
(define (get-data data pos)
  (if (equal? (list-ref data pos) "invalid")
      0
      (string->number (list-ref data pos))))

;; Returns the data point as a string
(define (get-data-string data pos) (list-ref data pos))

; Emulator internals printer for debugging
(define (print-emu ip prog data inputs)
  ; Helper to print each line
  (define (print-emu-line curr ip prog data inputs)
    (when (and (< curr 1000) (or (not (equal? (list-ref prog curr) "invalid")) (not (equal? (list-ref data curr) "invalid"))))
      (begin
        (display (string-pad (number->string curr) 3 "0") out-file)
        (display ": " out-file)
        (display (if (= curr ip) "> " "  ") out-file)
        (display (if (equal? (list-ref prog curr) "invalid") "           " (list-ref prog curr)) out-file)
        (display "  " out-file)
        (display (if (equal? (list-ref data curr) "invalid") "           " (list-ref data curr)) out-file)
        (display "  " out-file)
        (display (if (> (length inputs) curr) (list-ref inputs curr) "") out-file)
        (newline out-file)
        (print-emu-line (+ 1 curr) ip prog data inputs)
        )
      )
    )
  
  ; Display header
  (display "--------------------------------------------" out-file)
  (newline out-file)
  (display "       Program:     Data:        Input:" out-file)
  (newline out-file)
  (print-emu-line 0 ip prog data inputs)
  )

; ------------------------------
; Main emulator
(define (tick ip prog data inputs)
  ((let ([ins (substring (list-ref prog ip) 0 2)]
         [op1 (string->number (substring (list-ref prog ip) 2 5))]
         [op2 (string->number (substring (list-ref prog ip) 5 8))]
         [dest (string->number (substring (list-ref prog ip) 8 11))])
     (lambda () ; could be begin?
       ; Debugging
       ;(print-emu ip prog data inputs)
       
       ; What is the current instruction?
       (cond
         ; move
         [(equal? ins "+0") (tick (+ ip 1) prog (set-data data dest (get-data data op1)) inputs)]
         
         ; add
         [(equal? ins "+1") (tick (+ ip 1) prog (set-data data dest (+ (get-data data op1) (get-data data op2))) inputs)]
         
         ; multiply
         [(equal? ins "+2") (tick (+ ip 1) prog (set-data data dest (* (get-data data op1) (get-data data op2))) inputs)]
         
         ; square
         [(equal? ins "+3") (tick (+ ip 1) prog (set-data data dest (* (get-data data op1) (get-data data op1))) inputs)]
         
         ; equal test
         [(equal? ins "+4")
          (tick (if (= (get-data data op1) (get-data data op2)) ; Where we jump depends on test result
                    dest ; Passed, go to where it wants
                    (+ ip 1)) ; Failed, go to next instruction
                prog data inputs)]
         
         ; >= test
         [(equal? ins "+5") 
          (tick (if (>= (get-data data op1) (get-data data op2)) ; Where we jump depends on test result
                    dest ; Passed, go to where it wants
                    (+ ip 1)) ; Failed, go to next instruction
                prog data inputs)]
         
         ; array get (op1(op2)->dest)
         [(equal? ins "+6") (tick (+ ip 1) prog (set-data data dest (get-data data (+ op1 (get-data data op2)))) inputs)]
         
         ; increment and test (counter < max)
         [(equal? ins "+7")
          (tick (if (< (+ 1 (get-data data op1)) (get-data data op2)) ; If we've reached the loop limit don't do the jump
                    dest ; Counter still below desired, go back to start of loop
                    (+ ip 1)) ; Loop limit reached, go to next instruction
                prog
                (set-data data op1 (+ 1 (get-data data op1))) ; Increment counter
                inputs)]
         
         ; read input
         [(equal? ins "+8") (tick (+ ip 1) prog (set-data data dest (car inputs)) (cdr inputs))]
         
         ; end program
         [(equal? ins "+9") 0] ; successful end
         
         ; negate and move
         [(equal? ins "-0") (tick (+ ip 1) prog (set-data data dest (- 0 (get-data data op1))) inputs)]
         
         ; subtract
         [(equal? ins "-1") (tick (+ ip 1) prog (set-data data dest (- (get-data data op1) (get-data data op2))) inputs)]
         
         ; divide
         [(equal? ins "-2") (tick (+ ip 1) prog (set-data data dest (/ (get-data data op1) (get-data data op2))) inputs)]
         
         ; square root
         [(equal? ins "-3") (tick (+ ip 1) prog (set-data data dest (sqrt (get-data data op1))) inputs)]
         
         ; not equal test - what is this actually in PLT Scheme? <> doesn't seem to work
         ; so I just switched the then and else clauses :P
         [(equal? ins "-4")
          (tick (if (= (get-data data op1) (get-data data op2)) ; Where we jump depends on test results
                    (+ ip 1) ; Failed, go to next instruction
                    dest) ; Passed, go to where it wants
                prog data inputs)]
         
         ; < test
         [(equal? ins "-5")
          (tick (if (< (get-data data op1) (get-data data op2)) ; Where we jump depends on test results
                    dest ; Passed, go to where it wants
                    (+ ip 1)) ; Failed, go to next instrution
                prog data inputs)]
         
         ; array set (op1->op2(dest))
         [(equal? ins "-6") (tick (+ ip 1) prog (set-data data (+ op2 (get-data data dest)) (get-data data op1)) inputs)]
         
         ; print data
         [(equal? ins "-8")
          (display (get-data-string data op1) out-file)
          (newline out-file)
          (tick (+ ip 1) prog data inputs)]
         
         ; unrecognize/unsupported instruction
         [else 1]))))) ; Bad end to emulation


; Where to read/write?
(define argv (vector->list (current-command-line-arguments)))
(define in-file (if (and (>= (length argv) 1) (not (equal? (list-ref argv 0) "-")))
                    (open-input-file (list-ref argv 0)) ; File
                    (current-input-port))) ; Stdin
(define out-file (if (and (>= (length argv) 2) (not (equal? (list-ref argv 1) "-")))
                     (open-output-file (list-ref argv 1) #:exists 'replace) ; File
                     (current-output-port))) ; Stdout

; Process input file
(define (read-section data)
  (define line (read-line in-file))
  (if (or (eof-object? line) (equal? (substring line 0 11) "+9999999999"))
      data
      (read-section (append data (list (substring line 0 11))))
      )
  )

(define data (init-mem (read-section empty)))
(define prog (init-mem (read-section empty)))
(define inputs (read-section empty))

; Run the emulator
(when (= (tick 0 prog data inputs) 1)
  (display "Emulation failed"))

; Close the files
(close-output-port out-file)
(close-input-port in-file)
