extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
push 20
push 10
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, 0xA
mov [fmt + 5], eax
mov eax, 0x0
mov [fmt + 6], eax
push fmt
call _printf
add esp, 12
push 25
push 15
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, 0xA
mov [fmt + 5], eax
mov eax, 0x0
mov [fmt + 6], eax
push fmt
call _printf
add esp, 12
ret