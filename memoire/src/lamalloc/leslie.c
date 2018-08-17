#include <unistd.h>
#include "leslie.h"

#define MAX_MEM 1000
#define warm_boot(s) fprintf (stderr, "%s", s)

typedef struct hdr
{
  struct hdr *ptr;
  unsigned int size;
} HEADER;

//extern 
void *_heapstart, *_heapend;

static short memleft;
static HEADER *frhd;

void
cfree (char *ap)
{
  HEADER *nxt, *prev, *f;
  f = (HEADER *) ap - 1;
  memleft += f->size;

  if (frhd > f)
    {
      nxt = frhd;
      frhd = f;
      prev = f + f->size;
      if (prev == nxt)
	{
	  f->size += nxt->size;
	  f->ptr = nxt->ptr;
	}
      else
	f->ptr = nxt;		// ERROR: nxt->ptr;
      return;
    }

  // NOT USED: nxt=frhd;
  for (nxt = frhd; nxt && nxt < f; prev = nxt, nxt = nxt->ptr)
    {
      if (nxt + nxt->size == f)
	{
	  nxt->size += f->size;
	  f = nxt + nxt->size;
	  if (f == nxt->ptr)
	    {
	      nxt->size += f->size;
	      nxt->ptr = f->ptr;
	    }
	  return;
	}
    }

  prev->ptr = f;
  prev = f + f->size;
  if (prev == nxt)
    {
      f->size += nxt->size;
      f->ptr = nxt;
      return;
    }
  // ADDED
  else
    {
      f->ptr = nxt;
    }
}

char *
cmalloc (int nbytes)
{
  HEADER *nxt, *prev;
  int nunits;
  nunits = (nbytes + sizeof (HEADER) - 1) / sizeof (HEADER) + 1;

  for (prev = NULL, nxt = frhd; nxt; nxt = nxt->ptr)
    {
      if (nxt->size >= nunits)
	{
	  if (nxt->size > nunits)
	    {
	      nxt->size -= nunits;
	      nxt += nxt->size;
	      nxt->size = nunits;
	    }
	  else
	    {
	      if (prev == NULL)
		frhd = nxt->ptr;
	      else
		prev->ptr = nxt->ptr;
	    }
	  memleft -= nunits;
	  return ((char *) (nxt + 1));
	}
    }
  warm_boot ("Allocation Failed!\n");
  return NULL;
}

void
i_alloc (void)
{
  // Code changed to call sbrk
  _heapstart = sbrk (0);

  if (_heapstart == (void *) -1)
    warm_boot ("1. sbrk Failed!\n");

  _heapend = sbrk (MAX_MEM * sizeof (HEADER));

  if (_heapend == (void *) -1)
    warm_boot ("2. sbrk Failed!\n");

  frhd = (HEADER *) _heapstart;

  frhd->ptr = NULL;
  frhd->size = ((char *) &_heapend - (char *) &_heapstart);
  memleft = frhd->size;
}
