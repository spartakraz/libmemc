/* implementation file #2 for module `heap`
 */

#include "memc_includes.h"

#define prv_get_reserved_addr(aligned_addr_)              \
          ((aligned_addr_) - MEMC_OFFSET (MEMC_OFFSET_SIZE))

#define prv_read_reserved_mem(reserved_mem_)              \
          *(MEMC_OFFSET_PTR ((reserved_mem_)))

#define prv_write_to_reserved_mem(reserved_mem_, value_)  \
          (*(MEMC_OFFSET_PTR ((reserved_mem_))) = MEMC_OFFSET ((value_)))

static pthread_mutex_t mutex;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

memc_byte *
memc_heap_alloc_aligned (const memc_size nbytes,
                         const memc_data_alignment alignment)
{
  memc_error_reset ();
  if (!nbytes || !memc_is_power_of_two (MEMC_CONST_SIZE (alignment)))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_NULL_PTR (memc_byte));
    }
  if (MEMC_IS_INVALID_ALIGNMENT (alignment))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_NULL_PTR (memc_byte));
    }
  const memc_size padding_size = MEMC_CONST_SIZE ((2 * (MEMC_CONST_SIZE (alignment))) +
                                                  MEMC_VOID_PTR_SIZE +
                                                  MEMC_OFFSET_SIZE);
  const memc_size total_size = MEMC_CONST_SIZE (nbytes + padding_size);
  memc_byte * init_mem = MEMC_BYTE_PTR (memc_heap_alloc (total_size));
  if (MEMC_IS_NULL_PTR (memc_byte, init_mem))
    {
      return MEMC_NULL_PTR (memc_byte);
    }
  memc_lock_thread ();
  const memc_offset init_addr = MEMC_CONST_OFFSET (init_mem);
  const memc_offset max_addr = MEMC_CONST_OFFSET (init_addr + padding_size);
  const memc_offset aligned_addr = memc_is_aligned (max_addr, MEMC_CONST_SIZE (alignment))
          ? max_addr
          : MEMC_CONST_OFFSET (memc_align_down (max_addr,
                                                MEMC_CONST_SIZE (alignment)));
  memc_byte * aligned_mem = MEMC_BYTE_PTR (aligned_addr);
  if (MEMC_IS_NULL_PTR (memc_byte, aligned_mem))
    {
      memc_unlock_thread ();
      memc_return_on_error (memc_error_null_pointer, MEMC_NULL_PTR (memc_byte));
    }
  const memc_offset reserved_addr = MEMC_CONST_OFFSET (prv_get_reserved_addr (aligned_addr));
  memc_byte * reserved_mem = MEMC_BYTE_PTR (reserved_addr);
  if (MEMC_IS_NULL_PTR (memc_byte, reserved_mem))
    {
      memc_unlock_thread ();
      memc_return_on_error (memc_error_null_pointer, MEMC_NULL_PTR (memc_byte));
    }
  prv_write_to_reserved_mem (reserved_mem, init_addr);
  memc_unlock_thread ();
  
  return aligned_mem;
}

void
memc_heap_dealloc_aligned (memc_byte * aligned_mem)
{
  if (!MEMC_IS_NULL_PTR (memc_byte, aligned_mem))
    {
      memc_lock_thread ();
      const memc_offset aligned_addr = MEMC_CONST_OFFSET (aligned_mem);
      const memc_offset reserved_addr = MEMC_CONST_OFFSET (prv_get_reserved_addr (aligned_addr));
      memc_byte * reserved_mem = MEMC_BYTE_PTR (reserved_addr);
      const memc_offset init_addr = MEMC_CONST_OFFSET (prv_read_reserved_mem (reserved_mem));
      memc_byte * init_mem = MEMC_BYTE_PTR (init_addr);
      memc_unlock_thread ();
      memc_heap_dealloc (init_mem);
    }
}
