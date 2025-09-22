section .text
global _mainCRTStartup
_mainCRTStartup:
  mov rcx,-1
  mov rdx,0
  mov eax, 44
  syscall
  ret