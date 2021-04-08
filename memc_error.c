/* implementation file for module `error`
 */

#include "memc_includes.h"

#define prv_error_code_is_valid(ec_) (((const memc_error_code) (ec_) >= memc_error_none) \
                                      && ((const memc_error_code) (ec_) <= memc_error_unknown))

static memc_error_code prv_last_error;
static pthread_mutex_t mutex;

static memc_error_code prv_last_error = memc_error_none;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const memc_error_code
memc_error_get (void)
{
  return (const memc_error_code) prv_last_error;
}

void
memc_error_set (const memc_error_code ec)
{
  memc_lock_thread ();
  prv_last_error = prv_error_code_is_valid (ec) ? ec : memc_error_unknown;
  memc_unlock_thread ();
}

const char *
memc_error_to_str (const memc_error_code ec)
{
  static const char * const error_table[] = {
    "No error",
    "Invalid argument",
    "No memory",
    "Invalid offset",
    "(NULL) Pointer",
    "Unknown error"
  };
  const int32_t n = prv_error_code_is_valid (ec)
          ? (const int32_t) ec
          : (const int32_t) memc_error_unknown;
  return (const char *) error_table[n];
}
