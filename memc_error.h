/** Package: memc
 ** Module:  error (error handling routines)
 ** Version: 0.1
 */

typedef enum memc_error_code memc_error_code;

enum memc_error_code
{
  memc_error_none = 0x00U,
  memc_error_invalid_argument = 0x01U,
  memc_error_no_memory = 0x02U,
  memc_error_invalid_offset = 0x03U,
  memc_error_null_pointer = 0x04U,
  memc_error_unknown = 0x05U
};

extern const memc_error_code
memc_error_get (void);

extern void
memc_error_set (const memc_error_code);

#define memc_error_reset() memc_error_set (memc_error_none)

extern const char *
memc_error_to_str (const memc_error_code);
