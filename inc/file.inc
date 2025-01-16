section .data
  TCFLSH      EQU 0x540b

section .text
; reads n(rsi) bytes from STDIN and put them inside a buffer (rdi)
; @param  rdi: pointer to buffer
; @param  rsi: bytes to read
; @param  rax: bytes read
read_input:
  sub   rsp, 0x18
  
  cmp   rdi, 0
  jle   .error

  cmp   rsi, 0
  jl    .error

  mov   [rsp], rdi
  mov   qword [rsp+0x8], rsi

  mov   rax, SYS_READ
  mov   rdi, STDIN
  mov   rsi, [rsp]
  mov   rdx, qword [rsp+0x8]
  syscall

  cmp   rax, 0
  jl    .error

  ; save number of bytes read
  mov   qword [rsp+0x10], rax

  ; flush kernel input buffer if bytes read is equal to bytes to read
  ; as it could be a potential buffer overflow
  cmp   rax, qword [rsp+0x8]
  jne   .skip_flush

  mov   rax, SYS_IOCTL
  mov   rdi, STDIN
  mov   rsi, TCFLSH
  mov   rdx, 0
  syscall

.skip_flush:
  mov   rax, qword [rsp+0x10]

  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  add   rsp, 0x18
  ret

