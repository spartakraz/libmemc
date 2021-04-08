#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "memc_export.h"

static void
test1 (void)
{
  puts ("test1");
  memc_size len = 21;
  char *s = (char *) memc_heap_alloc (len);
  if (s == NULL)
    {
      puts (memc_error_to_str (memc_error_get ()));
      return;
    }
  strcpy (s, "test1: Hello, world!");
  puts (s);
  memc_heap_dealloc ((memc_byte *) s);
  puts (memc_error_to_str (memc_error_get ()));
}

static void
test2 (void)
{
  puts ("test2");
  char *s = (char *) memc_heap_alloc_aligned (21, memc_alignment_fundamental);
  if (s == NULL)
    {
      puts (memc_error_to_str (memc_error_get ()));
      return;
    }
  strcpy (s, "test2: Hello, world!");
  puts (s);
  memc_heap_dealloc_aligned ((memc_byte *) s);
  puts (memc_error_to_str (memc_error_get ()));
}

static void
test3 (void)
{
  puts ("test3");
  char *s;
  if ((memc_heap_memalign ((memc_byte **) & s, 21, memc_alignment_fundamental)) == MEMC_RC_FAILED)
    {
      puts (memc_error_to_str (memc_error_get ()));
      return;
    }
  if (s == NULL)
    {
      puts (memc_error_to_str (memc_error_get ()));
      return;
    }
  strcpy (s, "test3: Hello, world!");
  puts (s);
  memc_heap_memalign_free ((memc_byte **) & s);
  puts (memc_error_to_str (memc_error_get ()));
}

static void
test4 (void)
{
  puts ("test4");
  char *s = (char *) memc_mmap_alloc (21);
  if (s == NULL)
    {
      puts (memc_error_to_str (memc_error_get ()));
      return;
    }
  strcpy (s, "test4: Hello, world!");
  puts (s);
  memc_mmap_dealloc ((memc_byte *) s, 21);
  puts (memc_error_to_str (memc_error_get ()));
}

static void
test5 (void)
#define POINT_SIZE sizeof (Point)
#define N_POINTS 100
{
  puts ("test5");

  typedef struct
  {
    int32_t x, y, z;
  } Point;

  memc_region * region = memc_region_create (memc_region_location_heap);
  if (region == (memc_region *) NULL)
    {
      puts (memc_error_to_str (memc_error_get ()));
      puts ("test5(): error");
      abort ();
    }
  Point * points[N_POINTS];
  Point *ptr;
  int32_t i;
  for (i = 0; i < N_POINTS; i++)
    {
      memc_byte * mem;
      if (memc_region_request_mem (region, (memc_byte **) & mem, POINT_SIZE) == MEMC_RC_FAILED)
        {
          puts (memc_error_to_str (memc_error_get ()));
          puts ("test5(): error");
          abort ();
        }
      points[i] = (Point *) mem;
      ptr = points[i];
      ptr->x = i;
      ptr->y = i + 1;
      ptr->z = i + 2;
      printf ("%d %d %d\n", ptr->x, ptr->y, ptr->z);
    }
  memc_region_dispose ((memc_region **) & region);
  puts (memc_error_to_str (memc_error_get ()));
}

int
main (int argc, char ** argv)
{
  puts (memc_error_to_str (memc_error_get ()));
  test1 ();
  test2 ();
  test3 ();
  test4 ();
  test5 ();
  return 0;
}
