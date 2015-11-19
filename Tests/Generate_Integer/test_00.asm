extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
mov eax, 0x0
mov [fmt + 0], eax
push fmt
call _printf
add esp, 4
ret