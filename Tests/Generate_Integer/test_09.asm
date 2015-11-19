extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
push 3
push 2
push 1
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, ' %d'
mov [fmt + 5], eax
mov eax, 0xA
mov [fmt + 8], eax
mov eax, 0x0
mov [fmt + 9], eax
push fmt
call _printf
add esp, 16
mov eax, 0xA
mov [fmt + 0], eax
mov eax, 0x0
mov [fmt + 1], eax
push fmt
call _printf
add esp, 4
push 6
push 5
push 4
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, ' %d'
mov [fmt + 5], eax
mov eax, 0x0
mov [fmt + 8], eax
push fmt
call _printf
add esp, 16
ret