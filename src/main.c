#include <malloc.h>
#include <assert.h>

int main(){
    void *c = malloc(100);
    assert(c != NULL);
    return 0;
}
