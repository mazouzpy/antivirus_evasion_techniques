#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>


int main(){
    // shell code 
    unsigned char shell[] = {};
    //allocate mem with rwx pers
    void *exec_mem = mmap(NULL,sizeof(shell), PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE , -1 , 0);
    //check if the mem addr alllocate
    if (exec_mem == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    //copy shell to the mem
    memcpy(exec_mem,shell,sizeof(shell));
    //func to run shell in mem
    void (*execute)() = (void (*))exec_mem;
    //run func
    execute();
    //clean allocate mem
    munmap(exec_mem,sizeof(shell));
    return 0;
}