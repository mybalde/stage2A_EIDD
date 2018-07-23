#include <stdio.h>
#include "leslie.h"

int
main (void)
{
  char *p;
  i_alloc ();
  p = cmalloc (10 * sizeof (char));
  printf ("%p\n", p);
  p[0] = 'a';
  int i;
  for (i = 1; i < 10; i++)
    p[i] = 'a' + i;
  for (i = 0; i < 10; i++)
    printf ("%c ", p[i]);
  printf ("\n");

  cfree (p);
  printf ("%p\n", p);
  printf ("%c\n", p[3]);

  p = cmalloc (10 * sizeof (char));
  printf ("%p\n", p);

  return 0;
}
