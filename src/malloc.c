#include <malloc.h>
#include <assert.h>
#include <stdio.h>
const int MALLOC_ALIGN_MASK = 2 * (sizeof(INTERNAL_SIZE_T)) -1;
const int SIZE_SZ = (sizeof(INTERNAL_SIZE_T));
void *start = NULL;
struct malloc_state main_arena;
struct malloc_par mp_ = {
  .top_size = TOP_CHUNK_SIZE
};
static void *sysmalloc (INTERNAL_SIZE_T nb, mstate av) __attribute__((noinline));
static void malloc_init_state (mstate av) __attribute__((noinline));
static void unlink_chunk (mchunkptr p) __attribute__((noinline));
void *malloc (size_t bytes){
  INTERNAL_SIZE_T nb;
  INTERNAL_SIZE_T size;
  INTERNAL_SIZE_T remainder_size;

  mchunkptr       victim;
  mchunkptr       remainder;

  void *p;
  
  nb = (bytes + SIZE_SZ + MALLOC_ALIGN_MASK) < MINSIZE ? MINSIZE : (bytes + SIZE_SZ + MALLOC_ALIGN_MASK) & (~MALLOC_ALIGN_MASK);

  //first request
  if(main_arena.top == NULL){
    malloc_init_state(&main_arena);
    p = sysmalloc(nb, &main_arena);
    assert(p != NULL);
    assert(main_arena.top != NULL);
    return p;
  }

  //unsorted bin
  while ((victim = ((mchunkptr)bin_at(&main_arena, 1))->bk) != bin_at(&main_arena, 1)) {
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
  mchunkptr p;                 /* chunk corresponding to mem */
  INTERNAL_SIZE_T size;        /* its size */
  mchunkptr nextchunk;         /* next contiguous chunk */
  INTERNAL_SIZE_T nextsize;    /* its size */
  int nextinuse;               /* true if nextchunk is used */
  INTERNAL_SIZE_T prevsize;    /* size of previous contiguous chunk */
  mchunkptr bck;               /* misc temp for linking */
  mchunkptr fwd;               /* misc temp for linking */
  if(mem == NULL){
    return;
  }
  p = mem2chunk (mem);
  size = chunksize(p);
  nextchunk = chunk_at_offset(p, size);
  nextsize = chunksize(nextchunk);
  /* consolidate backward */
  if (!prev_inuse(p)) {
    prevsize = prev_size (p);
    size += prevsize;
    p = chunk_at_offset(p, -((long) prevsize));
    if (__glibc_unlikely (chunksize(p) != prevsize))
      malloc_printerr ("corrupted size vs. prev_size while consolidating");
    unlink_chunk (p);
  }
  if (nextchunk != main_arena.top) {
    /* get and clear inuse bit */
    nextinuse = inuse_bit_at_offset(nextchunk, nextsize);
  
      /* consolidate forward */
      if (!nextinuse) {
	      unlink_chunk (nextchunk);                                     
	      size += nextsize;
      } else
	      clear_inuse_bit_at_offset(nextchunk, 0);
      bck = bin_at(&main_arena, 1);
      fwd = bck->fd;
      //if (__glibc_unlikely (fwd->bk != bck))
	//malloc_printerr ("free(): corrupted unsorted chunks");
      p->fd = fwd;
      p->bk = bck;
      bck->fd = p;
      fwd->bk = p;

      set_head(p, size | PREV_INUSE);
      set_foot(p, size);
      //check_free_chunk(av, p);
    }
    /*
      If the chunk borders the current high end of memory,
      consolidate into top
    */
    else {
      size += nextsize;
      set_head(p, size | PREV_INUSE);
      main_arena.top = p;
      //check_chunk(av, p);
    }
}

void *calloc(size_t count, size_t size) { TODO(); return NULL;}
void *realloc(void *ptr, size_t size) { TODO(); return NULL;}
void *reallocf(void *ptr, size_t size) { TODO(); return NULL;}
void *valloc(size_t size) { TODO(); return NULL;}
void *aligned_alloc(size_t alignment, size_t size) { TODO(); return NULL;}
static void
unlink_chunk (mchunkptr p)
{
  if (chunksize (p) != prev_size (next_chunk (p)))
    malloc_printerr ("corrupted size vs. prev_size");

  mchunkptr fd = p->fd;                                                 
  mchunkptr bk = p->bk;

  if (__builtin_expect (fd->bk != p || bk->fd != p, 0))
    malloc_printerr ("corrupted double-linked list");

  fd->bk = bk;
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
    set_foot(p, nb);
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