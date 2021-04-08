/** Package: memc
 ** Module:  mmap (mmap allocation routines)
 ** Version: 0.1
 */

extern memc_byte *
memc_mmap_alloc (const memc_size);

extern const memc_return_code
memc_mmap_dealloc (memc_byte *,
                   const memc_size);
