/** Package: memc
 ** Module:  region (region management routines)
 ** Version: 0.1
 */

#include "memc_includes.h"

typedef struct memc_region memc_region;

typedef enum memc_region_location memc_region_location;

enum memc_region_location
{
  memc_region_location_heap = 0x01U,
  memc_region_location_mmap = 0x02U
};

#define PRV_IS_INVALID_REGION_LOCATION(location_) (!MEMC_BIT_IS_SET (location_, memc_region_location_heap) \
                                                    && !MEMC_BIT_IS_SET (location_, memc_region_location_mmap))

#if defined(DEBUG)
#define PRV_MAX_BLOCK_COUNT 10
#define PRV_PAGE_SIZE 256
#else
#define PRV_MAX_BLOCK_COUNT 20
#define PRV_PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

#define PRV_REGION_ALIGNMENT (4 * MEMC_VOID_PTR_SIZE)

#define PRV_NULL_BLOCK MEMC_NULL_PTR (prv_block_s)
#define PRV_NULL_REGION MEMC_NULL_PTR (memc_region)

#define PRV_REGION_STRUCT_SIZE(alignment) memc_align_up (sizeof (memc_region), alignment)
#define PRV_BLOCK_STRUCT_SIZE(alignment)  memc_align_up (sizeof (prv_block_s), alignment)
#define PRV_REGION_HEADER_SIZE(alignment)   (PRV_REGION_STRUCT_SIZE (alignment) + PRV_BLOCK_STRUCT_SIZE (alignment))

#define prv_init_block(block_, next_, buffer_start_, buffer_end_)   \
{                                                                   \
  block_->next          = next_;                                    \
  block_->buffer.start  = buffer_start_;                            \
  block_->buffer.end    = buffer_end_;                              \
}

#define prv_append_block(region_, block_)         \
{                                                 \
  block_->next = prv_create_new_block (region_);  \
  region_->last_block = block_->next;             \
  block_ = region_->last_block;                   \
  region_->block_count++;                         \
}

#define prv_init_region(region_,                \
                            first_block_,       \
                            last_block_,        \
                            alignment_,         \
                            page_size_,         \
                            block_count_,       \
                            location_,          \
                            block_size_,        \
                            region_size_)       \
{                                                                                                                                           \
  region_->first_block  = (first_block_);                                                                                                   \
  region_->last_block   = (last_block_);                                                                                                    \
  region_->alignment    = (alignment_);                                                                                                     \
  region_->page_size    = (page_size_);                                                                                                     \
  region_->block_count  = (block_count_);                                                                                                   \
  region_->location     = (location_);                                                                                                      \
  region_->block_size   = (block_size_);                                                                                                    \
  region_->region_size  = (region_size_);                                                                                                   \
}

typedef struct prv_block_s prv_block_s;

struct prv_block_s
{
  prv_block_s * next;

  struct
  {
    memc_byte * start;
    memc_byte * end;
  } buffer;
};

struct memc_region
{
  prv_block_s * first_block;
  prv_block_s * last_block;
  memc_size page_size;
  memc_size alignment;
  memc_size block_count;
  memc_size block_size;
  memc_size region_size;
  memc_region_location location;
};

static pthread_mutex_t mutex;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static prv_block_s *
prv_create_new_block (memc_region *);
static memc_region *
prv_create_new_region (const memc_data_alignment,
                       const memc_size,
                       const memc_region_location);

extern memc_region *
memc_region_create (const memc_region_location);
extern void
memc_region_dispose (memc_region **);
extern const memc_return_code
memc_region_request_mem (memc_region *,
                         memc_byte **,
                         const memc_size);

static prv_block_s *
prv_create_new_block (memc_region * region)
{
  memc_error_reset ();
  if (MEMC_IS_NULL_PTR (memc_region, region))
    {
      memc_return_on_error (memc_error_invalid_argument, PRV_NULL_BLOCK);
    }
  if (region->block_count >= PRV_MAX_BLOCK_COUNT)
    {
      memc_return_on_error (memc_error_no_memory, PRV_NULL_BLOCK);
    }
  const memc_size page_size = MEMC_CONST_SIZE (region->page_size);
  const memc_size alignment = MEMC_CONST_SIZE (region->alignment);
  const memc_size struct_size = MEMC_CONST_SIZE (PRV_BLOCK_STRUCT_SIZE (alignment));
  const memc_size nbytes = struct_size + page_size;
  memc_byte * mem = MEMC_BIT_IS_SET (region->location, memc_region_location_heap)
          ? MEMC_BYTE_PTR (memc_heap_alloc (nbytes))
          : MEMC_BYTE_PTR (memc_mmap_alloc (nbytes));
  if (MEMC_IS_NULL_PTR (memc_byte, mem) || MEMC_IS_MAP_FAILED_PTR (memc_byte, mem))
    {
      memc_return_on_error (memc_error_no_memory, PRV_NULL_BLOCK);
    }
  prv_block_s * block = (prv_block_s *) mem;
  if (MEMC_IS_NULL_PTR (prv_block_s, block))
    {
      memc_return_on_error (memc_error_null_pointer, PRV_NULL_BLOCK);
    }
  memc_byte * buffer_start = MEMC_BYTE_PTR (mem + struct_size);
  if (MEMC_IS_NULL_PTR (memc_byte, buffer_start))
    {
      memc_return_on_error (memc_error_null_pointer, PRV_NULL_BLOCK);
    }
  memc_byte * buffer_end = MEMC_BYTE_PTR (mem + nbytes);
  if (MEMC_IS_NULL_PTR (memc_byte, buffer_end))
    {
      memc_return_on_error (memc_error_null_pointer, PRV_NULL_BLOCK);
    }
  prv_init_block (block, PRV_NULL_BLOCK, buffer_start, buffer_end);
  if (!region->block_size)
    {
      region->block_size = nbytes;
    }
#if defined(DEBUG)
  memc_debug_trace_event (memc_event_block_created);
#endif

  return block;
}

static memc_region *
prv_create_new_region (const memc_data_alignment alignment,
                       const memc_size page_size,
                       const memc_region_location location)
{
  memc_error_reset ();
  if (MEMC_IS_INVALID_ALIGNMENT (alignment)
      || MEMC_IS_INVALID_SIZE (page_size)
      || !memc_is_power_of_two (MEMC_CONST_SIZE (page_size))
      || PRV_IS_INVALID_REGION_LOCATION (location))
    {
      memc_return_on_error (memc_error_invalid_argument, PRV_NULL_REGION);
    }
  const memc_size region_heap_size = page_size;
  const memc_size total_size = PRV_REGION_HEADER_SIZE (MEMC_CONST_SIZE (alignment)) +
          region_heap_size;
  memc_byte * allocated_mem = MEMC_BIT_IS_SET (location, memc_region_location_heap)
          ? MEMC_BYTE_PTR (memc_heap_alloc (total_size))
          : MEMC_BYTE_PTR (memc_mmap_alloc (total_size));
  const memc_offset region_struct_addr = MEMC_CONST_OFFSET (allocated_mem);
  memc_region * region = (memc_region *) region_struct_addr;
  if (MEMC_IS_NULL_PTR (memc_region, region))
    {
      memc_return_on_error (memc_error_invalid_offset, PRV_NULL_REGION);
    };
  const memc_offset block_struct_addr = region_struct_addr +
          PRV_REGION_STRUCT_SIZE (MEMC_CONST_SIZE (alignment));
  prv_block_s * block = (prv_block_s *) block_struct_addr;
  if (MEMC_IS_NULL_PTR (prv_block_s, block))
    {
      memc_return_on_error (memc_error_invalid_offset, PRV_NULL_REGION);
    };
  const memc_offset buffer_start_addr = block_struct_addr +
          PRV_BLOCK_STRUCT_SIZE (MEMC_CONST_SIZE (alignment));
  memc_byte * buffer_start = MEMC_BYTE_PTR (buffer_start_addr);
  if (MEMC_IS_NULL_PTR (memc_byte, buffer_start))
    {
      memc_return_on_error (memc_error_invalid_offset, PRV_NULL_REGION);
    };
  const memc_offset buffer_end_addr = region_struct_addr + total_size;
  memc_byte * buffer_end = MEMC_BYTE_PTR (buffer_end_addr);
  if (MEMC_IS_NULL_PTR (memc_byte, buffer_end))
    {
      memc_return_on_error (memc_error_invalid_offset, PRV_NULL_REGION);
    };
  prv_init_block (block,
                  PRV_NULL_BLOCK,
                  buffer_start,
                  buffer_end);
  prv_init_region (region,
                   block,
                   region->first_block,
                   MEMC_CONST_SIZE (alignment),
                   page_size,
                   1,
                   location,
                   0,
                   total_size);
#if defined(DEBUG)
  memc_debug_trace_event (memc_event_block_created);
  memc_debug_trace_region (region);
#endif

  return region;
}

extern memc_region *
memc_region_create (const memc_region_location location)
{
  return prv_create_new_region (memc_alignment_fundamental, MEMC_CONST_SIZE (PRV_PAGE_SIZE), location);
}

extern void
memc_region_dispose (memc_region ** region)
{
  if (!MEMC_IS_NULL_PTR (memc_region, *region))
    {
      memc_assert ((*region)->block_size || (*region)->region_size);
      memc_lock_thread ();
      prv_block_s * ptr = (*region)->first_block;
      prv_block_s * tmp;
      prv_block_s * cursor;
      cursor = ptr->next;
      while (!MEMC_IS_NULL_PTR (prv_block_s, cursor))
        {
          tmp = cursor->next;
          if (MEMC_BIT_IS_SET ((*region)->location, memc_region_location_heap))
            {
              memc_heap_dealloc (MEMC_BYTE_PTR (cursor));
            }
          else
            {
              memc_mmap_dealloc (MEMC_BYTE_PTR (cursor), (*region)->block_size);
            }
          (*region)->block_count--;
#if defined(DEBUG)
          memc_debug_trace_event (memc_event_block_disposed);
          memc_debug_trace_region ((*region));
#endif
          cursor = tmp;
        }
      if (MEMC_BIT_IS_SET ((*region)->location, memc_region_location_heap))
        {
          memc_heap_dealloc (MEMC_BYTE_PTR (*region));
        }
      else
        {
          memc_mmap_dealloc (MEMC_BYTE_PTR (*region), (*region)->region_size);
        }
      memc_unlock_thread ();
    }
}

extern const memc_return_code
memc_region_request_mem (memc_region * region, memc_byte ** mem, const memc_size requested_size)
{
#define get_avail_buffer_space(block_) \
            MEMC_CONST_SIZE ((block_)->buffer.end - (block_)->buffer.start)

  memc_error_reset ();
  if (MEMC_IS_NULL_PTR (memc_region, region) || MEMC_IS_INVALID_SIZE (requested_size))
    {
      memc_return_on_error (memc_error_invalid_argument, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }
  prv_block_s * current_block = region->last_block;
  if (MEMC_IS_NULL_PTR (prv_block_s, current_block))
    {
      memc_return_on_error (memc_error_null_pointer, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }
  memc_lock_thread ();
  const memc_size allocated_size = memc_align_up (requested_size, region->alignment);
  if (allocated_size > get_avail_buffer_space (current_block))
    {
      prv_append_block (region, current_block);
#if defined(DEBUG)
      memc_debug_trace_region (region);
#endif
    }
  if (MEMC_IS_NULL_PTR (prv_block_s, current_block))
    {
      memc_unlock_thread ();
      memc_return_on_error (memc_error_null_pointer, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }
  current_block->buffer.start += allocated_size;
  *mem = MEMC_BYTE_PTR ((MEMC_OFFSET (current_block->buffer.start - allocated_size)));
  memc_unlock_thread ();
  if (MEMC_IS_NULL_PTR (memc_byte, *mem))
    {
      memc_return_on_error (memc_error_null_pointer, MEMC_CONST_RETURN_CODE (MEMC_RC_FAILED));
    }

  return MEMC_CONST_RETURN_CODE (MEMC_RC_OK);
}
