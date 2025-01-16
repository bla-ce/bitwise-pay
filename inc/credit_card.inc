section .data
  MIN_CREDIT_CARD_NUMBER equ 0x10
  MAX_CREDIT_CARD_NUMBER equ 0x13

  VISA_CREDIT_CARD_LENGTHS  dq 13, 16, 19, 0
  VISA_PREFIX               dq 4 

section .text

; reads the credit card number pointed by rdi and return the issuing network
; @param  rdi: buffer containing credit card number
; @return rax: pointer to issuing network
; supports Visa and Mastercard cards only
; https://en.wikipedia.org/wiki/Payment_card_number#Issuer_identification_number_(IIN)
read_credit_card_number:
  sub   rsp, 0x8

  mov   [rsp], rdi

  ; check if visa
  mov   rdi, [rsp]
  call  is_visa_card

  ; check if mastercard

  ; return error

  jmp   .return

.error:
  mov   rax, FAILURE_CODE 

.return:
  add   rsp, 0x8
  ret

; checks if the credit card number corresponds to a Visa credit card
; @param  rdi: pointer to credit card number
; @return rax: is visa vard
is_visa_card:
  sub   rsp, 0x8

  mov   [rsp], rdi

  ; check length of the card
  call  strlen

  cmp   rax, 0
  jl    .error

  lea   rdi, [VISA_CREDIT_CARD_LENGTHS]

.loop:
  mov   rbx, qword [rdi]
  cmp   rbx, rax
  je    .end_loop

  add   rdi, 0x8
  cmp   rbx, 0
  je    .not_visa

  jmp   .loop
  
.end_loop:
  ; check if first digit is 4 
  mov   rsi, [rsp]
  xor   rdi, rdi
  mov   dil, byte [rsi]
  call  ascii_to_int

  cmp   rax, 0
  jl    .error

  cmp   rax, qword [VISA_PREFIX]
  jne   .not_visa

  mov   rax, 1

  jmp   .return

.not_visa:
  mov   rax, 0
  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  add   rsp, 0x8
  ret

