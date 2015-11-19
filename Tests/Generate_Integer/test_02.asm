extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
push 10
push 2
pop ebx
pop eax
imul eax, ebx
push eax
push 2
pop ebx
pop eax
add eax, ebx
push eax
mov eax, '%d'
mov [fmt + 0], eax
mov eax, 0x0
mov [fmt + 2], eax
push fmt
call _printf
add esp, 8
ret