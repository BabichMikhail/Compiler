extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
push 10
push 20
pop ebx
pop eax
imul eax, ebx
push eax
push 99
pop ebx
pop eax
xor edx, edx
div ebx
push edx
push 10
push 20
pop ebx
pop eax
add eax, ebx
push eax
push 30
push 30
pop ebx
pop eax
xor edx, edx
div ebx
push eax
pop ebx
pop eax
add eax, ebx
push eax
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, 0x0
mov [fmt + 5], eax
push fmt
call _printf
add esp, 12
ret