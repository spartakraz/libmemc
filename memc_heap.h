/** Package: memc
 ** Module:  heap (heap allocation routines)
 ** Version: 0.1
 */

extern memc_byte *
memc_heap_alloc (const memc_size);

extern void
memc_heap_dealloc (memc_byte *);

extern memc_byte *
memc_heap_alloc_aligned (const memc_size,
                         const memc_data_alignment);

extern void
memc_heap_dealloc_aligned (memc_byte *);
