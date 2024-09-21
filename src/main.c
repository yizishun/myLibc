#include <malloc.h>
#include <assert.h>
#include <stdio.h>

extern struct malloc_state main_arena;
extern struct malloc_par mp_;
int main(){
    void *a = malloc(0x20);
    printf("a = %p\n",a);
    printf("main_arena = %p\n",&main_arena);
    free(a);
    return 0;
}
