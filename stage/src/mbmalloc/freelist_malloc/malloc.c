#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "malloc.h"

typedef struct header {
  size_t	  size;        /* memory block size */
  struct header  *nxt;
} header;

/* Global limits of the heap */
void* global_base = NULL;
void* global_end = NULL;

/* Head of the free list */
header* frhd = NULL;

#define warm_boot(s) fprintf (stderr, "%s", s)
#define BLOCK_SIZE sizeof(header)

/* Interface for the header's information */
#define HDR_GET_SIZE(p) 	(p->size & (~1))
#define HDR_GET_STATUS(p) 	(p->size & 1)
#define HDR_GET_NEXT(p) 	((header*)p + HDR_GET_SIZE(p))
#define HDR_SET_SIZE(p,nh)	p->size = (((nh + 1) >> 1) << 1) & (~HDR_GET_STATUS(p))
#define HDR_SET_STATUS(p)   p->size = p->size | 1
#define HDR_UNSET_STATUS(p) p->size = p->size & (~1)

/* Forward declarations of utilities */
void    cscan(char*);
void	defrag(header *);
header* search(size_t);

void
cfree(void *p)
{
  header* block = (header *) p;
  block = block - 1;
  /* pointer on current block and last block */
  header* ptr;

#ifdef VERBOSE_M
    printf("\n  - search address %p\n", block);
#endif

  /* looking for if the block exists */
  for (ptr = (header*) global_base; ptr < (header*) global_end;
       ptr = HDR_GET_NEXT(ptr)) {
#ifdef VERBOSE_M
    printf("\t- visit address %p\n", ptr);
#endif
    if (ptr == block) {
      /* already free: error */
      if (HDR_GET_STATUS(ptr)) {
        warm_boot("free failed!\n");
	return;
      }

      /* set status to free */
      HDR_SET_STATUS(ptr);
      if (frhd == NULL) {
	/* empty free list */
        frhd = ptr;
	frhd->nxt = NULL;
      } else if (frhd < ptr) {
        /* non empty free list */
        /* insert such that the free list is sorted by address */
	header *tmp = frhd;

#ifdef VERBOSE_M
        printf("   - insert in free list %p\n", frhd);
#endif
        assert(tmp != NULL && tmp < ptr);
	while (tmp->nxt != NULL && tmp->nxt < ptr) {
#ifdef VERBOSE_M
          printf("\t-visit free chunk %p\n", tmp->nxt);
#endif
	  tmp = tmp->nxt;
	}

	ptr->nxt = tmp->nxt;
	tmp->nxt = ptr;
      } else {
        assert (frhd > ptr);
	block->nxt = frhd;
	frhd = block;
      }
      cscan("After free");
      return;
    }
  }
  warm_boot("free failed!\n");
}

void*
cmalloc(size_t size)
{
  header* block = NULL;
  /* compute the size in header blocks */
  size_t rsize = ((size / BLOCK_SIZE) + 1) + 1;
  /* make it even to have the last bit unused */
  rsize = ((rsize & 1) == 1) ? (rsize + 1) : rsize;

  if (global_base != NULL && frhd != NULL) {
    /* non empty free list */
    /* search a free chunk that fits the request, uses First Fit Algorithm */
    block = search(rsize);
    if (block == NULL) {
      /* no free chunk fits, defragmentate the free list */
      defrag(frhd);
      /* redo the search */
      block = search(rsize);
    }
    if (block)
      return (void *)(block + 1);
  }

  /* block is still null */
  assert (block == NULL);

  /* extend the memory */
  if (global_base == NULL) {
    global_base = sbrk(0);
    global_end = global_base ;
  }
  /* the block will be at the end of the current region */
  block = global_end;
  /* extend the memory by a system call */
  if (sbrk(rsize * BLOCK_SIZE) == (void *)-1)
    /* sbrk failed */
    return NULL;

  block->size = rsize;
  /* HDR_GET_STATUS(p) is occupied, i.e., 0 */
  /* update the limit of the memory region */
  global_end = sbrk(0);

  return (void *)(block + 1);
}

void
cscan(char* msg)
{
#ifdef VERBOSE_M
  header* ptr = global_base;
  printf("* %s:\n", msg);
  printf("  - [%p, %p)\n", global_base, global_end);
  for (; ((void *)ptr) != global_end; ptr = HDR_GET_NEXT(ptr))
    printf("Chunk %p: size %ld (=%ld B), status %ld\n", ptr, HDR_GET_SIZE(ptr),
	   HDR_GET_SIZE(ptr) * BLOCK_SIZE,
	   HDR_GET_STATUS(ptr));
  printf("-----------------------------------------------------\n");
#endif
  return;
}

void
defrag(header* frhd)
{
  header* ptr = frhd;

  assert (ptr != NULL);
  while (ptr->nxt != NULL) {
    if (HDR_GET_NEXT(ptr) == ptr->nxt) {
      /* for two consecutive free blocks, do coalescing */
      /* increase size */
      ptr->size = HDR_GET_SIZE(ptr) + HDR_GET_SIZE(ptr->nxt);
      /* set again as free */
      HDR_SET_STATUS(ptr);
      /* update the next pointer */
      ptr->nxt = (ptr->nxt)->nxt;
    } else {
      ptr->nxt = (ptr->nxt)->nxt;
    }
  }
  cscan("After defrag");
}

header*
search(size_t rsize)
{
  header* block, *prev;

  for (prev = NULL, block = frhd; block != NULL;
       prev = block, block = block->nxt) {
    assert (HDR_GET_STATUS(block) == 1);
    if (HDR_GET_SIZE(block) >= rsize) {
      if (HDR_GET_SIZE(block) == rsize) {
	/* no need to split */
        block->size = rsize;
	/* remove block from free list */
        if (prev)
	  prev->nxt = block->nxt;
	else
	  frhd = block->nxt;
	return block;
      } else {
	/* split the block, reserve its end for this allocation */
        header* rblock = block + (HDR_GET_SIZE(block) - rsize);

	rblock->size = rsize;
        /* update the size of the free block */
	block->size = HDR_GET_SIZE(block) - rsize;
        /* let block in the free list */
        HDR_SET_STATUS(block);
        cscan("After split");
        return rblock;
      }
    }
  }
  return NULL;
}
