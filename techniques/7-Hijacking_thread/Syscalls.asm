.code
    NtSetContextThread proc
        mov r10, rcx
        mov eax, 0x05
        syscall
        ret
    NtSetContextThread endp
end