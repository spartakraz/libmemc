/* implementation file #1 for module `mmap`
 */

#include "memc_includes.h"

extern memc_byte *
memc_mmap_alloc (const memc_size nbytes)
{
#define PROT_FLAGS  (PROT_READ | PROT_WRITE)
#define MAP_FLAGS   (MAP_PRIVATE | MAP_ANONYMOUS)

  memc_error_reset ();
  if (MEMC_IS_INVALID_SIZE (nbytes))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_NULL_PTR (memc_byte));
    }
  void * mem = mmap (NULL, nbytes, PROT_FLAGS, MAP_FLAGS, 0, 0);
  if (MEMC_IS_MAP_FAILED_PTR (void, mem))
    {
      memc_return_on_error (memc_error_no_memory, MEMC_NULL_PTR (memc_byte));
    }

  return MEMC_BYTE_PTR (mem);
}

extern const memc_return_code
memc_mmap_dealloc (memc_byte * mem, 
                   const memc_size nbytes)
{
  memc_error_reset ();
  if (MEMC_IS_INVALID_SIZE (nbytes) 
       || MEMC_IS_NULL_PTR (memc_byte, mem) 
       || MEMC_IS_MAP_FAILED_PTR (memc_byte, mem))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }
  const int32_t rc = (const int32_t) munmap (MEMC_VOID_PTR (mem), nbytes);
  if (rc)
    {
      memc_return_on_error (memc_error_no_memory, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }

  return MEMC_CONST_RETURN_CODE (MEMC_RC_OK);
}
