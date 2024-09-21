#ifndef MALLOC_H
#define MALLOC_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

#define TOP_CHUNK_SIZE 0x200000
void *malloc(size_t);
void free(void *);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocf(void *ptr, size_t size);
void *valloc(size_t size);
void *aligned_alloc(size_t alignment, size_t size);

//type-----------------------------------------------------------------------------------------------
typedef size_t INTERNAL_SIZE_T;
#define SIZE_SZ (sizeof(INTERNAL_SIZE_T))

//chunk------------------------------------------------------------------------------------------------
struct malloc_chunk {

  INTERNAL_SIZE_T      mchunk_prev_size;  /* Size of previous chunk (if free).  */
  INTERNAL_SIZE_T      mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};
typedef struct malloc_chunk* mchunkptr;
//frontend----------------
// Size and alignment checks and conversions
#define chunk2mem(p) ((void *) ((char *) (p) + 2 * SIZE_SZ))
#define mem2chunk(mem) ((mchunkptr)((char *) (mem) -2 * SIZE_SZ))

#define MIN_CHUNK_SIZE  (sizeof(struct malloc_chunk))
#define MINSIZE MIN_CHUNK_SIZE
#define MALLOC_ALIGN_MASK 2 * SIZE_SZ -1
#define aligned_OK(m) (((unsigned long) (m) & MALLOC_ALIGN_MASK) == 0)

#define request2size(req)                                         \
  (((req) + SIZE_SZ + MALLOC_ALIGN_MASK < MINSIZE)  ?             \
   MINSIZE :                                                      \
   ((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)

//backend-----------------(size, prev_size, prev_inuse, inuse(out), chunk)
//get size
#define chunksize(p) (chunksize_nomask (p) & ~(SIZE_BITS))
#define chunksize_nomask(p)         ((p)->mchunk_size)
#define prev_size(p) ((p)->mchunk_prev_size)

//set size
#define set_head_size(p, s)  ((p)->mchunk_size = (((p)->mchunk_size & SIZE_BITS) | (s)))
#define set_head(p, s)       ((p)->mchunk_size = (s))
#define set_foot(p, s)       (((mchunkptr) ((char *) (p) + (s)))->mchunk_prev_size = (s))

//set prev size
#define set_prev_size(p, sz) ((p)->mchunk_prev_size = (sz))

//get prev inuse
#define PREV_INUSE 0x1
#define prev_inuse(p)       ((p)->mchunk_size & PREV_INUSE)
#define SIZE_BITS (PREV_INUSE)

//get/set/clear inuse
#define inuse(p)							      \
  ((((mchunkptr) (((char *) (p)) + chunksize (p)))->mchunk_size) & PREV_INUSE)
#define set_inuse(p)							      \
  ((mchunkptr) (((char *) (p)) + chunksize (p)))->mchunk_size |= PREV_INUSE
#define clear_inuse(p)							      \
  ((mchunkptr) (((char *) (p)) + chunksize (p)))->mchunk_size &= ~(PREV_INUSE)

//get prev/next chunk
#define next_chunk(p) ((mchunkptr) (((char *) (p)) + chunksize (p)))
#define prev_chunk(p) ((mchunkptr) (((char *) (p)) - prev_size (p)))
#define chunk_at_offset(p, s)  ((mchunkptr) (((char *) (p)) + (s)))


#pragma GCC poison mchunk_size
#pragma GCC poison mchunk_prev_size
//state------------------------------------------------------------------------------------------------------
typedef struct malloc_chunk *mbinptr;
struct malloc_state
{
  mchunkptr top;
  mchunkptr bins[2];
};
typedef struct malloc_state* mstate;
static struct malloc_state main_arena;
#define bin_at(m, i) \
  (mbinptr) (((char *) &((m)->bins[((i) - 1) * 2]))			      \
             - offsetof (struct malloc_chunk, fd))

//parameter---------------------------------------------------------------------------------------------------
struct malloc_par
{
  INTERNAL_SIZE_T top_size;
};
static struct malloc_par mp_ = {
  .top_size = TOP_CHUNK_SIZE
};

//utils--------------------------------------------------------------------------------------------------------
#define panic_on(cond, s) \
  ({ if (cond) { \
      printf("Libc : %s\n", s); \
      exit(1); \
    } })

#define malloc_printerr(s) panic_on(1, s)
#define TODO() panic_on(1, "not implemented")
#endif