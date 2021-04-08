/* implementation file #3 for module `heap`
 */

#include "memc_includes.h"

static pthread_mutex_t mutex;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern const memc_return_code
memc_heap_memalign (memc_byte **,
                const memc_size,
                const memc_data_alignment);
extern void
memc_heap_memalign_free (memc_byte **);

extern const memc_return_code
memc_heap_memalign (memc_byte ** mem, const memc_size nbytes, const memc_data_alignment alignment)
{
  memc_byte * ptr = MEMC_BYTE_PTR (memc_heap_alloc_aligned (nbytes, alignment));
  if (ptr == MEMC_NULL_PTR (memc_byte))
    {
      return MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED);
    }
  memc_lock_thread ();
  *mem = ptr;
  memc_unlock_thread ();
  
  return MEMC_CONST_RETURN_CODE (MEMC_RC_OK);
}

extern void
memc_heap_memalign_free (memc_byte ** mem)
{
  memc_heap_dealloc_aligned (*mem);
}
