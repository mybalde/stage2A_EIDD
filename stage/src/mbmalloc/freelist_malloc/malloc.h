#ifndef MALLOC_H_INCLUDED
#define MALLOC_H_INCLUDED

void *cmalloc (size_t size);
void cfree (void *p);
void cscan ();
void cscanfree ();
#endif // MALLOC_H_INCLUDED
