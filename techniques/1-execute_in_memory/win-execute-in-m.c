#include <windows.h>
#include <stdio.h>


int main(){

    //shell bytes
    unsigned char shell[] = {};

    //allocate mem with execute write read perms
    LPVOID exec_mem = VirtualAlloc(NULL,sizeof(shell),MEM_COMMIT | MEM_RESERVE , PAGE_EXECUTE_READWRITE);
    
    //check if the mem addr is allocate
    if (exec_mem == NULL){
        printf("Error : Could not allocate memory\n");
        return 1;
    }
    //copy shell to allocate mem 
    memcpy(exec_mem, shell , sizeof(shell));

    //func to points to allocate mem
    void (*execute)() = (void (*)())exec_mem;
    //run function
    execute();
    //clean allocate  mem
    VirtualFree(exec_mem,0,MEM_RELEASE);
}
