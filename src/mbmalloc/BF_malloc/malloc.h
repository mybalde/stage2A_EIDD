#ifndef MALLOC_H_INCLUDED
#define MALLOC_H_INCLUDED

void *cmalloc (size_t size);
void cfree (void *p);
void cscan ();

#endif // MALLOC_H_INCLUDED
