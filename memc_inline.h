/** Package:    memc
 ** Module:     <none>
 ** Purpose:    inline functions shared by different modules
 ** Version:    0.1
 */

static inline const bool
memc_is_power_of_two (const memc_size);
static inline const bool
memc_is_aligned (const memc_offset,
                 const memc_size);

static inline const bool
memc_is_power_of_two (const memc_size n)
{
  return MEMC_CONST_BOOL ((n && !(n & (n - 1))));
}

static inline const bool
memc_is_aligned (const memc_offset value,
                 const memc_size alignment)
{
  return MEMC_CONST_BOOL (((value % alignment) == 0));
}