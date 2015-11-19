extern _printf
section .data
	fmt : times 128 db 0
section .text
global _main
_main:
mov eax, 0xA
mov [fmt + 0], eax
mov eax, 0x0
mov [fmt + 1], eax
push fmt
call _printf
add esp, 4
push 105
push 91
push 78
push 66
push 55
push 45
push 36
push 28
push 21
push 15
push 10
push 6
push 3
push 1
mov eax, '%d'
mov [fmt + 0], eax
mov eax, ' %d'
mov [fmt + 2], eax
mov eax, ' %d'
mov [fmt + 5], eax
mov eax, ' %d'
mov [fmt + 8], eax
mov eax, ' %d'
mov [fmt + 11], eax
mov eax, ' %d'
mov [fmt + 14], eax
mov eax, ' %d'
mov [fmt + 17], eax
mov eax, ' %d'
mov [fmt + 20], eax
mov eax, ' %d'
mov [fmt + 23], eax
mov eax, ' %d'
mov [fmt + 26], eax
mov eax, ' %d'
mov [fmt + 29], eax
mov eax, ' %d'
mov [fmt + 32], eax
mov eax, ' %d'
mov [fmt + 35], eax
mov eax, ' %d'
mov [fmt + 38], eax
mov eax, 0x0
mov [fmt + 41], eax
push fmt
call _printf
add esp, 60
ret