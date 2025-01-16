%include "malloc.inc"
%include "free.inc"
%include "mmap.inc"
%include "syscall.inc"
%include "constants.inc"
%include "credit_card.inc"
%include "utils.inc"
%include "file.inc"
%include "ascii.inc"
%include "string.inc"

section .data
  PROMPT_ENTER_CC db "Enter Credit Card Number: ", NULL_CHAR

  BUFFER_LENGTH equ 0x40

section .text
global _start

_start:
  sub   rsp, 0x8

  lea   rdi, [PROMPT_ENTER_CC]
  mov   rsi, 0
  call  print

  ; malloc string for storing credit card number, add one for null
  mov   rdi, BUFFER_LENGTH
  inc   rdi
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  ; read credit card number
  mov   rdi, rax
  mov   rsi, BUFFER_LENGTH
  call  read_input

  cmp   rax, 0
  jl    .error

  ; add null char at the end of the credit card number
  ; any character after will be discarded
  mov   rdi, [rsp]
  add   rdi, rax
  dec   rdi ; go before new line
  xor   rax, rax
  stosb

  mov   rdi, [rsp]
  call  read_credit_card_number

  ; free buffer
  mov   rdi, [rsp]
  call  free

.return:
  add   rsp, 0x8

  mov   rax, SYS_EXIT
  mov   rdi, SUCCESS_CODE
  syscall

.error:
  add   rsp, 0x8

  mov   rax, SYS_EXIT
  mov   rdi, FAILURE_CODE
  syscall

