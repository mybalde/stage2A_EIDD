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


#define warm_boot(s) fprintf (stderr, "%s", s)
#define BLOCK_SIZE sizeof(header)

// methods to get header information
#define HDR_GET_SIZE(p) 	(p->size & (~1))
#define HDR_GET_STATUS(p) 	(p->size & 1)
#define HDR_GET_NEXT(p) 	((header*) p + HDR_GET_SIZE(p))
#define HDR_SET_SIZE(p,nh)	p->size = (((nh + 1) >> 1) << 1) & (~HDR_GET_STATUS(p))
#define HDR_SET_STATUS(p)   p->size = p->size | 1
#define HDR_UNSET_STATUS(p) p->size = p->size & (~1)

inline int
hdr_get_size (header * p)
{
    return p->size & (~1);
}


void
cscan ()
{
    header *ptr = global_base;
    for (; ((void *) ptr) != global_end; ptr = HDR_GET_NEXT (ptr))
        printf ("Chunk %p: size %ld, status %ld\n", ptr, HDR_GET_SIZE (ptr),
                HDR_GET_STATUS (ptr));
    printf ("-----------------------------------\n");
}

void *
cmalloc (size_t size)
{
    header *block, *best_block;

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

    //finding a chunk with the Best Fit Algorithm
    for (block = global_base; block != (header *) global_end;
            block = HDR_GET_NEXT (block))
    {

        if (HDR_GET_STATUS (block) && HDR_GET_SIZE (block) >= rsize)
        {
            if (!best_block
                    || (block && HDR_GET_SIZE (block) < HDR_GET_SIZE (best_block)))
                best_block = block;
        }
    }

    if(best_block)
        block=best_block;

    // Failed to find free block
    if (block == (header *) global_end )
    {
        block = sbrk (0);
        // sbrk failed
        if (sbrk (rsize * BLOCK_SIZE) == (void *) -1)
            return NULL;

        block->size = rsize;
        if (global_base == NULL)
            global_base = block;
        global_end = sbrk (0);
    }
    else				// Found free block
    {
        if (HDR_GET_SIZE (block) == rsize)
        {
            block->size = rsize;
        }
        else
        {
            //splitting block
            header *rblock = block + rsize;

            rblock->size = (block->size - rsize) | 1;
            block->size = rsize & (~1);	// set status to 0

            /* HDR_SET_SIZE(rblock, HDR_GET_SIZE(block)-rsize);
               HDR_SET_STATUS(rblock);
               HDR_UNSET_STATUS(block); */
        }
    }

    return (void *) (block + 1);
}

void
cfree (void *p)
{
    header *block = (header *) p;
    block = block - 1;
    //pointer on current block and last block
    header *ptr, *last, *nxt;

    //looking for wether the block exists
    for (ptr = (header *) global_base; ptr < (header *) global_end;
            ptr = HDR_GET_NEXT (ptr))
    {
        if (ptr == block)
        {
            //already free
            if (HDR_GET_STATUS (ptr))
                return;
            //coalescing previous and current blocks
            if (last && HDR_GET_STATUS (last))
            {
                last->size = (last->size & (~1)) + (ptr->size & (~1));
                ptr = last;
            }
            //coalescing next and current blocks
            if (ptr != (header *) global_end)
            {
                nxt = HDR_GET_NEXT (ptr);
                if (nxt && HDR_GET_STATUS (nxt))
                {
                    ptr->size = ptr->size + (nxt->size & (~1));
                }
            }
            // set status to 1
            ptr->size = ptr->size | 1;

            //printf ("free of %p\n\n", p);

            return;
        }
        last = ptr;
    }

    warm_boot ("free failed!\n");
    return;
}
