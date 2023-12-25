		global _start

section .text
_start:
		push rbp
		mov rbp, rsp
		mov rax,1				; [1] - sys_write
		mov rdi,1				; 0 = stdin / 1 = stdout / 2 = stderr
		lea rsi,[rel msg]     	; pointer(mem address) to msg (*char[])
		mov rdx, msg_end - msg	; msg size
		syscall					; calls the function stored in rax
		call _wsh
		mov rsp, rbp
		pop rbp
		mov rax, 60
		xor rdi, rdi
		syscall

		;mov rax, 0x11111111
		;jmp rax

_wsh:
		push rbp
		mov rbp, rsp
		mov 	rax, 1 				; write syscall
		mov 	rdi, 1 				; 1 = stdout
		lea 	rsi, [rel ft_msg]		; pointer to ft_msg
		mov 	rdx, 36 ;size
		syscall
		pop rbp
		ret


align 8
		msg     db 'This file has been infected by halvarez',0x0a,0
		msg_end db 0x0
		ft_msg	db	'This is a msg from another function', 0x0a, 0
