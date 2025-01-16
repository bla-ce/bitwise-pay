%include "syscall.inc"
%include "constants.inc"

section .text
global _start

_start:
  mov   rax, SYS_EXIT
  mov   rdi, SUCCESS_CODE
  syscall

