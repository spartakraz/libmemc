/* implementation file #1 for module `heap`
 */

#include "memc_includes.h"

#define prv_clear_mem(mem_, len_) memset ((mem_), 0, (len_))

memc_byte *
memc_heap_alloc (const memc_size nbytes)
{
#define malloc_size(nbytes)   (MEMC_CONST_SIZE ((nbytes) + MEMC_BYTE_SIZE))

  memc_error_reset ();
  if (MEMC_IS_INVALID_SIZE (nbytes))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_NULL_PTR (memc_byte));
    }
  void *mem = MEMC_VOID_PTR (malloc (malloc_size (nbytes)));
  if (MEMC_IS_NULL_PTR (void, mem))
    {
      memc_return_on_error (memc_error_no_memory, MEMC_NULL_PTR (memc_byte));
    }
  (void) prv_clear_mem (mem, nbytes);

  return MEMC_BYTE_PTR (mem);
}

void
memc_heap_dealloc (memc_byte * mem)
{
  if (!MEMC_IS_NULL_PTR (memc_byte, mem))
    {
      free (MEMC_VOID_PTR (mem));
    }
}
