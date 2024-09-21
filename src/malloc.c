#include <malloc.h>
#include <assert.h>
#include <stdio.h>
void *start = NULL;
static void *sysmalloc (INTERNAL_SIZE_T nb, mstate av);
static void malloc_init_state (mstate av);
static void unlink_chunk (mchunkptr p);
void *malloc (size_t bytes){
  INTERNAL_SIZE_T nb;
  INTERNAL_SIZE_T size;
  INTERNAL_SIZE_T remainder_size;

  mchunkptr       victim;
  mchunkptr       remainder;
  mchunkptr       fwd;
  mchunkptr       bck;

  void *p;
  
  malloc_init_state(&main_arena);

  nb = request2size(bytes);
  //first request
  if(main_arena.top == NULL){
    p = sysmalloc(nb, &main_arena);
    assert(p != NULL);
    assert(main_arena.top != NULL);
    return p;
  }

  //unsorted bin
  while ((victim = (mchunkptr)main_arena.bins[0]->bk) != main_arena.bins[0]) {
    size = chunksize(victim);
    /* split */
    if(size >= nb){
      if(size - nb >= MINSIZE){
        remainder_size = size - nb;
        remainder = victim;
        victim = chunk_at_offset(remainder, remainder_size);
        set_head(victim, nb);
        set_inuse(victim);
        set_head_size(remainder, remainder_size);
        set_foot(remainder, remainder_size);
        p = chunk2mem(victim);
        return p;
      }
      else{
        unlink_chunk(victim);
        set_inuse(victim);
        return chunk2mem(victim);
      }
    }
  }
  if(nb > chunksize(main_arena.top) - MINSIZE) TODO();
  /* split */
  else{
    victim = main_arena.top;
    size = chunksize(victim);
    remainder_size = size - nb;
    remainder = chunk_at_offset (victim, nb);
    main_arena.top = remainder;
    set_head (victim, nb | PREV_INUSE);
    set_head (remainder, remainder_size | PREV_INUSE);
    void *p = chunk2mem (victim);
    return p;
  }
  //can't reach here
  assert(0);
  return NULL;
}

void free(void *mem)
{
}

void *calloc(size_t count, size_t size) { TODO(); return NULL;}
void *realloc(void *ptr, size_t size) { TODO(); return NULL;}
void *reallocf(void *ptr, size_t size) { TODO(); return NULL;}
void *valloc(size_t size) { TODO(); return NULL;}
void *aligned_alloc(size_t alignment, size_t size) { TODO(); return NULL;}
static void
unlink_chunk (mchunkptr p)
{
  if (chunksize (p) != prev_size (next_chunk (p)))                      //一，对分别对该chunk和上一个chunk的size和presize进行对比检测
    malloc_printerr ("corrupted size vs. prev_size");

  mchunkptr fd = p->fd;                                                 
  mchunkptr bk = p->bk;

  if (__builtin_expect (fd->bk != p || bk->fd != p, 0))                 //二，对p的双向链表指针进行检测
    malloc_printerr ("corrupted double-linked list");

  fd->bk = bk;                                                          //三，解开p的fd和bk
  bk->fd = fd;
}
static void *
sysmalloc (INTERNAL_SIZE_T nb, mstate av){
  INTERNAL_SIZE_T size;
  mchunkptr p;

  size = nb + mp_.top_size;
  if(av->top == NULL){
    start = sbrk(0);
    p = sbrk(size);
    main_arena.top = chunk_at_offset(p, nb);
    set_head(p, nb | PREV_INUSE);
    set_head(main_arena.top, mp_.top_size | PREV_INUSE);
    return chunk2mem(p);
  }
  else{
    TODO();
  }
}
static void
malloc_init_state (mstate av)
{
  int i;
  mbinptr bin;

  /* Establish circular links for normal bins */
  for (i = 1; i < 2; ++i)
    {
      bin = bin_at (av, i);
      bin->fd = bin->bk = bin;
    }
}