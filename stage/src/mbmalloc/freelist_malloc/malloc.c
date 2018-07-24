#include <unistd.h>
#include <stdio.h>
#include "malloc.h"

typedef struct header
{
    size_t size;			//memory block size
    struct header *nxt;
} header;

//head of the list
void *global_base = NULL;
void *global_end = NULL;

header *frhd;

#define warm_boot(s) fprintf (stderr, "%s", s)
#define BLOCK_SIZE sizeof(header)

// methods to get header information
#define HDR_GET_SIZE(p) 	(p->size & (~1))
#define HDR_GET_STATUS(p) 	(p->size & 1)
#define HDR_GET_NEXT(p) 	((header*) p + HDR_GET_SIZE(p))
#define HDR_SET_SIZE(p,nh)	p->size = (((nh + 1) >> 1) << 1) & (~HDR_GET_STATUS(p))
#define HDR_SET_STATUS(p)   p->size = p->size | 1
#define HDR_UNSET_STATUS(p) p->size = p->size & (~1)

void defrag (header * );
header* search (size_t);

void
cfree (void *p)
{
    header *block = (header *) p;
    block = block - 1;
    //pointer on current block and last block
    header *ptr;

    //looking for wether the block exists
    for (ptr = (header *) global_base; ptr != (header *) global_end;
            ptr = HDR_GET_NEXT (ptr))
    {
        if (ptr == block)
        {
            //already free
            if (HDR_GET_STATUS (ptr))
                return;

            HDR_SET_STATUS (ptr);	// set status to 1
            //free list is null
            if (frhd == NULL)
            {
                frhd = ptr;
                return;
            }
            else if (frhd < ptr)
            {
                header *tmp;
                tmp = frhd;
                
                while (tmp->nxt && tmp < ptr)
                    tmp = tmp->nxt;
                
                ptr->nxt = tmp->nxt;
                tmp->nxt = ptr;
                return;
            }
            else if (frhd > ptr)
            {
                block->nxt = frhd;
                frhd = block;
                return;
            }
        }

    }
    warm_boot ("free failed!\n");
}

void *cmalloc (size_t size)
{
    header *block = NULL;
    // size in header blocks
    size_t rsize = (size / BLOCK_SIZE + 1) + 1;
    rsize = ((rsize & 1) == 1) ? rsize + 1 : rsize;

    // First call
    if (!global_base)
    {
        block = sbrk (0);
        // sbrk failed
        if (sbrk (rsize * BLOCK_SIZE) == (void *) -1)
            return NULL;

        block->size = rsize;

        global_base = block;
        global_end = sbrk (0);

        return (void *) (block + 1);
    }
    //finding a chunk with the First Fit Algorithm
    if (frhd)
    {
        block = search(rsize);
        if (block == NULL)
        {
			defrag (frhd);
			block = search(rsize);
		}
		if (block)
			 return (void *) (block + 1);
    }

		// Failed to find free block
			block = global_end;
			block = sbrk (0);
		  // sbrk failed
		  if (sbrk (rsize * BLOCK_SIZE) == (void *) -1)
		return NULL;

		  block->size = rsize;

		  global_end = sbrk (0);
    


    return (void *) (block + 1);
}

void cscan ()
{
    header *ptr = global_base;
    for (; ((void *) ptr) != global_end; ptr = HDR_GET_NEXT (ptr))
        printf ("Chunk %p: size %ld, status %ld\n", ptr, HDR_GET_SIZE (ptr),
                HDR_GET_STATUS (ptr));
    printf ("-----------------------------------------------------\n");
}

void defrag (header * frhd)
{
    header *ptr = frhd; 
    
    while(ptr->nxt) 
    {
        if (ptr + HDR_GET_SIZE (ptr) == ptr->nxt)
        {
            ptr->size = HDR_GET_SIZE (ptr) + HDR_GET_SIZE (ptr->nxt);
            HDR_SET_STATUS(ptr);
            ptr->nxt = (ptr->nxt)->nxt;
        }
		else
			ptr->nxt = (ptr->nxt)->nxt;
    }
}

header* search (size_t rsize)
{
	header* block, *prev;
	
    for (prev=NULL, block = frhd; block; prev=block, block = block->nxt)
   {
            if (HDR_GET_STATUS (block) && HDR_GET_SIZE (block) >= rsize)
            {
                if (HDR_GET_SIZE (block) == rsize)
                {
                    block->size = rsize;
                    //remove block from free list
                    if (prev)
                        prev->nxt = block->nxt;
                    else
                        frhd = block->nxt;
                    
                    return block;
                }
                else
                {
                    //splitting block
					header *rblock = block + (HDR_GET_SIZE (block)-rsize);

					rblock->size = rsize ;
					block->size = (HDR_GET_SIZE (block)-rsize) | 1;	// set status to 0
					
					return rblock;
                }

			}

    }
    return NULL;
}
