// size of a (void*) pointer on your machine
#define MEMC_VOID_PTR_SIZE          sizeof (void *)

// emulation of java's byte type
typedef uint8_t memc_byte;

// use it instead of size_t
typedef size_t memc_size;

typedef enum memc_error_code memc_error_code;
typedef enum memc_return_code memc_return_code;
typedef enum memc_data_alignment memc_data_alignment;
typedef enum memc_region_location memc_region_location;

// specifies a memory region
typedef struct memc_region memc_region;

// possible values of the library's errno
enum memc_error_code
{
  memc_error_none = 0x00U,
  memc_error_invalid_argument = 0x01U,
  memc_error_no_memory = 0x02U,
  memc_error_invalid_offset = 0x03U,
  memc_error_null_pointer = 0x04U,
  memc_error_unknown = 0x05U
};

// return code of a function
enum memc_return_code
{
  MEMC_RC_OK = 0x00U,
  MEMC_RC_FAILED = 0x01U
};

// data alignment
enum memc_data_alignment
{
  // 16-byte alignment
  memc_alignment_fundamental = (uint32_t) (2 * MEMC_VOID_PTR_SIZE),
  // 32-byte alignment
  memc_alignment_extended = (uint32_t) (4 * MEMC_VOID_PTR_SIZE)
};

// where a memory region is to be allocated
enum memc_region_location
{
  // heap  
  memc_region_location_heap = 0x01U,
  // mmap
  memc_region_location_mmap = 0x02U
};

// version of malloc() which returns a byte array in heap
extern memc_byte *
memc_heap_alloc (const memc_size);

// version of free() which frees a byte array in heap
extern void
memc_heap_dealloc (memc_byte *);

// Allocates a byte array of the given size and alignment in heap.
// In case of error, returns `(memc_byte *) NULL`.
// You can get the error's code using `memc_error_get()`.
// WARN: you free the allocated byte array only thru `memc_heap_dealloc_aligned()`
extern memc_byte *
memc_heap_alloc_aligned (const memc_size,
                         const memc_data_alignment);

// Frees a byte array returned by `memc_heap_alloc_aligned()`
extern void
memc_heap_dealloc_aligned (memc_byte *);

// This is something like `posix_memalign()` for allocating an aligned
// memory in heap. On success, returns MEMC_RC_OK. ON failure, 
// returns MEMC_RC_FAILED. 
// You can get the error's code using `memc_error_get()`.
// WARN: you free the allocated byte array only thru `memc_heap_memalign_free()`
extern const memc_return_code
memc_heap_memalign (memc_byte **,
                    const memc_size,
                    const memc_data_alignment);
// Frees a byte array allocated thru `memc_heap_dealloc_aligned()`
extern void
memc_heap_memalign_free (memc_byte **);

// a simple wrapper around mmap()
extern memc_byte *
memc_mmap_alloc (const memc_size);

// a simple wrapper around munmap()
extern const memc_return_code
memc_mmap_dealloc (memc_byte *,
                   const memc_size);

// Creates a memory region (arena) in the given place
// in memory (heap, mmap). On success, returns the
// created region. On failure, returns (memc_region *) NULL.
// You can get the error's code using `memc_error_get()`.
extern memc_region *
memc_region_create (const memc_region_location);

// Frees the region created thru `memc_region_create()`
extern void
memc_region_dispose (memc_region **);

// Allocates a memory of the given size for parameter `mem`. The
// memory is allocated from the given region.
// On success returns MEMC_RC_OK. On failure, returns MEMC_RC_FAILED.
// You can get the error's code using `memc_error_get()`.
extern const memc_return_code
memc_region_request_mem (memc_region *region,
                         memc_byte ** mem,
                         const memc_size size);

// gets the current status of the library's errno
extern const memc_error_code
memc_error_get (void);

// gets the description of an error
extern const char *
memc_error_to_str (const memc_error_code);
