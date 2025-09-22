section .text
global _mainCRTStartup
_mainCRTStartup:
  mov rax,0x0000003319801129
  loop:
  dec rax
  cmp rax, 0
  jne loop
