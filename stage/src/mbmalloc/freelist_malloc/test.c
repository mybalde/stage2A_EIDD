#include <stdio.h>
#include "CUnit/Basic.h"
#include "malloc.c"

int
init_suite_cm ()
{
    return 0;
}

int
clean_suite_cm ()
{
    return 0;
}

void
test_couverture_instruction ()
{
    void *v = cmalloc (10);
    void *v1 = cmalloc (10);
    cfree (v1);
    v1 = cmalloc (10);
    cfree (v);
    cfree (v1);
};

void
test_couverture_mcdc ()
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

};

void
test_free ()
{
    // precondition fonctionnement normal
    void *v = cmalloc (10);
    //tests
    cfree (v);
    header *mcb;
    mcb = v - sizeof (header);
    CU_ASSERT_EQUAL (HDR_GET_STATUS (mcb), 1);
}



void
test_malloc ()
{
    int taille = 10;
    void *v = cmalloc (taille);
    CU_ASSERT_PTR_NOT_NULL (global_base);
    CU_ASSERT_PTR_NOT_NULL (global_end);
    // même si il n'appartiens pas au test, on effectue un free
    cfree (v);
}

int
main ()
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry ())
        return CU_get_error ();

    /* add a suite to the registry
     * The 1st test suite corresponds to the global functions, with a cover
     */
    CU_pSuite pSuite =
        CU_add_suite ("test_suite_couverture", init_suite_cm, clean_suite_cm);
    if (NULL == pSuite)
    {
        CU_cleanup_registry ();
        return CU_get_error ();
    }

    /* add the tests global to the suite */
    if (NULL ==
            CU_add_test (pSuite, "test_couverture_instruction",
                         test_couverture_instruction)
            || NULL == CU_add_test (pSuite, "test_couverture_mcdc",
                                    test_couverture_mcdc))
    {
        CU_cleanup_registry ();
        return CU_get_error ();
    }
	printf ("\n");
    /* add a suite to the registry
     * The 2nd test suite corresponds to the individual functions
     */
    CU_pSuite pSuite2 =
        CU_add_suite ("individual test suite in BF malloc", init_suite_cm, clean_suite_cm);
    if (NULL == pSuite2)
    {
        CU_cleanup_registry ();
        return CU_get_error ();
    }
    /* add the tests malloc to the suite */
    if (NULL == CU_add_test (pSuite2, "test of malloc", test_malloc))
    {
        CU_cleanup_registry ();
        return CU_get_error ();
    }

    /* add the tests free to the suite */
    if (NULL == CU_add_test (pSuite2, "test of free", test_free))
    {
        CU_cleanup_registry ();
        return CU_get_error ();
    }



    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode (CU_BRM_VERBOSE);
    CU_basic_run_tests ();
    printf ("\n");
    CU_basic_show_failures (CU_get_failure_list ());
    printf ("\n");

    /* Clean up registry and return */
    CU_cleanup_registry ();
    return CU_get_error ();
}
