#include <malloc.h>
#include <assert.h>
void *malloc (size_t bytes){
    printf("malloc called\n");
    assert(0);
    return NULL;
}

void free(void *mem)
{
}
