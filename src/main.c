#include <malloc.h>
#include <assert.h>
#include <stdio.h>

int main(){
    void *c = malloc(100);
    assert(c != NULL);
    printf("end\n");
    return 0;
}
