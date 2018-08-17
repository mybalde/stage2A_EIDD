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
	CU_ASSERT_PTR_NULL (global_base);
    CU_ASSERT_PTR_NULL (global_end);
    void *v = cmalloc (10);
    CU_ASSERT_PTR_NOT_NULL (global_base);
    CU_ASSERT_PTR_NOT_NULL (global_end);
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
  a = cmalloc (3 * sizeof (int));
  b = cmalloc (7 * sizeof (int));
  c = cmalloc (2 * sizeof (int));
  d = cmalloc (2 * sizeof (int));
  
  cfree (b);
  cfree (a);
  cfree (d);
  cfree (c);
  
  e = cmalloc (2 * sizeof (int));
  a = cmalloc (200 * sizeof (int));
  
  cfree (a);
  
  a = cmalloc (20 * sizeof (int));
  
  cfree (a);
 cfree(e);
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
    CU_add_suite ("individual test suite in FF malloc", init_suite_cm,
		  clean_suite_cm);
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
