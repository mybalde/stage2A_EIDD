#include <stdio.h>
#include "malloc.h"

int
main ()
{
  int *a, *b, *c, *d, *e;

  printf ("-----------------------------------------------------\n");
  a = cmalloc (3 * sizeof (int));
  cscan ();
  printf ("a : adresse %p\n", a);

  printf ("-----------------------------------------------------\n");
  b = cmalloc (7 * sizeof (int));
  cscan ();
  printf ("\nb : adresse %p\n", b);

  printf ("-----------------------------------------------------\n");
  c = cmalloc (2 * sizeof (int));
  cscan ();
  printf ("\nc : adresse %p\n", c);

  printf ("-----------------------------------------------------\n");
  d = cmalloc (2 * sizeof (int));
  cscan ();
  printf ("\nd : adresse %p\n", c);

  printf ("-----------------------------------------------------\n");
  printf ("\nb : free\n\n");
  cfree (b);
  cscan ();

  printf ("-----------------------------------------------------\n");
  printf ("\na : free\n\n");
  cfree (a);
  cscan ();

  printf ("-----------------------------------------------------\n");
  printf ("\nd : free\n\n");
  cfree (d);
  cscan ();

  printf ("-----------------------------------------------------\n");
  printf ("\nc : free\n\n");
  cfree (c);
  cscan ();

  printf ("-----------------------------------------------------\n");
  e = cmalloc (2 * sizeof (int));
  cscan ();
  printf ("\ne : adresse %p\n", c);

  printf ("-----------------------------------------------------\n");
  a = cmalloc (200 * sizeof (int));
  cscan ();
  printf ("\na : adresse %p\n", c);

  printf ("-----------------------------------------------------\n");
  printf ("\na : free\n\n");
  cfree (a);
  cscan ();

  printf ("-----------------------------------------------------\n");
  a = cmalloc (20 * sizeof (int));
  cscan ();
  printf ("\na : adresse %p\n", c);

  printf ("-----------------------------------------------------\n");
  printf ("\na : free\n\n");
  cfree (a);
  cscan ();

  return 0;
}
