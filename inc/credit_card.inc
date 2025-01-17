section .data
  MIN_CREDIT_CARD_NUMBER equ 0x10
  MAX_CREDIT_CARD_NUMBER equ 0x13

  VISA_CREDIT_CARD_LENGTHS  dq 13, 16, 19, 0
  VISA_PREFIX               equ 4
  IS_VISA                   db "Visa Card", NULL_CHAR

  MASTERCARD_CREDIT_CARD_LENGTH   equ 16
  MASTERCARD_PREFIX_LOWER_BOUND_1 equ 51
  MASTERCARD_PREFIX_UPPER_BOUND_1 equ 55
  MASTERCARD_PREFIX_LOWER_BOUND_2 equ 2221
  MASTERCARD_PREFIX_UPPER_BOUND_2 equ 2720
  IS_MASTERCARD                   db "Mastercard Card", NULL_CHAR

  IS_UNKNOWN  db "Unknown", NULL_CHAR

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

  cmp   rax, 1
  je    .is_visa

  mov   rdi, [rsp]
  call  is_mastercard_card

  cmp   rax, 1
  je    .is_mastercard

  ; return error
  jmp   .is_unknown

.is_visa:
  lea   rax, [IS_VISA]
  jmp   .return

.is_mastercard:
  lea   rax, [IS_MASTERCARD]
  jmp   .return

.is_unknown:
  lea   rax, [IS_UNKNOWN]
  jmp   .return
  
.error:
  mov   rax, FAILURE_CODE 

.return:
  add   rsp, 0x8
  ret

; checks if the credit card number corresponds to a MasterCard credit card
; @param  rdi: pointer to credit card number
; @return rax: is mastercard card
is_mastercard_card:
  sub   rsp, 0x20

  mov   [rsp], rdi
  mov   qword [rsp+0x8], 0
  mov   qword [rsp+0x10], 0

  ; check length of the card
  call  strlen

  cmp   rax, 0
  jl    .error

  cmp   rax, MASTERCARD_CREDIT_CARD_LENGTH
  jne   .is_not_mastercard_card

  ; get first two numbers
  mov   rdi, 2 
  call  malloc

  cmp   rax, 0
  jl    .error
  
  mov   [rsp+0x8], rax

  mov   rdi, rax
  mov   rsi, [rsp]
  mov   rcx, 2
  rep   movsb

  ; convert string to int
  mov   rdi, [rsp+0x8]
  call  stoi

  cmp   rax, 0
  jl    .error

  cmp   rax, MASTERCARD_PREFIX_LOWER_BOUND_1
  jl    .check_bounds_2

  cmp   rax, MASTERCARD_PREFIX_UPPER_BOUND_1
  jg    .check_bounds_2

  jmp   .is_mastercard_card

.check_bounds_2:
  ; get first 4 numbers
  mov   rdi, 4
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [rsp+0x10], rax

  mov   rdi, rax
  mov   rsi, [rsp]
  mov   rcx, 4
  rep   movsb

  ; convert string to int
  mov   rdi, [rsp+0x10]
  call  stoi

  cmp   rax, 0
  jl    .error

  cmp   rax, MASTERCARD_PREFIX_LOWER_BOUND_2
  jl    .is_not_mastercard_card

  cmp   rax, MASTERCARD_PREFIX_UPPER_BOUND_2
  jg    .is_not_mastercard_card

; save return value
.is_mastercard_card:
  mov   qword [rsp+0x18], 1
  jmp   .free

.is_not_mastercard_card:
  mov   qword [rsp+0x18], 0
  jmp   .free

.error:
  mov   qword [rsp+0x18], FAILURE_CODE

.free:
  ; free string
  cmp   qword [rsp+0x8], 0
  je    .return

  mov   rdi, [rsp+0x8]
  call  free

  cmp   qword [rsp+0x10], 0
  je    .return

  mov   rdi, [rsp+0x10]
  call  free  

.return:
  mov   rax, qword [rsp+0x18]
  add   rsp, 0x20
  ret

; checks if the credit card number corresponds to a Visa credit card
; @param  rdi: pointer to credit card number
; @return rax: is visa card
is_visa_card:
  sub   rsp, 0x8

  mov   [rsp], rdi

  ; check length of the card
  call  strlen

  cmp   rax, 0
  jl    .error

  xor   rdi, rdi
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

  cmp   rax, VISA_PREFIX
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

