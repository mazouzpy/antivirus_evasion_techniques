.code
    NtAllocateVirtualMemmory PROC
        mov r10, rcx
        mov eax, 0 ; you nedd to know syscall id and change 0 with it
        syscall
        ret
    NtAllocateVirtualMemmory ENDP

END
