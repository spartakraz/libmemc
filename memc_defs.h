/** Package:    memc
 ** Module:     <none>
 ** Purpose:    definitions shared by different modules
 ** Version:    0.1
 */

#if !defined(NULL)
#define NULL ((void *) 0)
#endif

#define MEMC_VOID_PTR_SIZE          MEMC_CONST_SIZE (sizeof (void *))
#define MEMC_OFFSET_SIZE            MEMC_CONST_SIZE (sizeof (memc_offset))
#define MEMC_BYTE_SIZE              (sizeof (memc_byte))

#define MEMC_NULL_PTR(TYPE_)        ((TYPE_ *) NULL)
#define MEMC_MAP_FAILED_PTR(TYPE_)  ((TYPE_ *) MAP_FAILED)
#define MEMC_CONST_BOOL(V)          ((const bool) (V))
#define MEMC_INT32(V)               ((const int32_t) (V))
#define MEMC_SIZE(V)                ((memc_size) (V))
#define MEMC_CONST_SIZE(V)          ((const memc_size) (V))
#define MEMC_OFFSET(V)              ((memc_offset) (V))
#define MEMC_CONST_OFFSET(V)        ((const memc_offset) (V))
#define MEMC_OFFSET_PTR(V)          ((memc_offset *) (V))
#define MEMC_BYTE_PTR(V)            ((memc_byte *) (V))
#define MEMC_VOID_PTR(V)            ((void *) (V))
#define MEMC_MUTEX_PTR(V)           ((pthread_mutex_t *) &(V))
#define MEMC_CONST_RETURN_CODE(rc)  ((const memc_return_code) (rc))

#define MEMC_IS_NULL_PTR(TYPE_, PTR_)           ((TYPE_ *) (PTR_) == MEMC_NULL_PTR (TYPE_))
#define MEMC_IS_MAP_FAILED_PTR(TYPE_, PTR_)     ((TYPE_ *) (PTR_) == MEMC_MAP_FAILED_PTR (TYPE_))

#define MEMC_BIT_IS_SET(VAR_, BITNO_)           ((VAR_) & (BITNO_))

#define MEMC_IS_INVALID_SIZE(s)                 (!(s))
#define MEMC_IS_INVALID_ALIGNMENT(alignment)    (((alignment) != memc_alignment_fundamental) \
                                                    && ((alignment) != memc_alignment_extended))

#define memc_align_down(n_, align_)             ((n_) - ((n_) % (align_)))
#define memc_align_up(n_, align_)               ((((n_) + (align_) - 1) / (align_)) * (align_))

#define memc_lock_mutex(mutex_)                 (pthread_mutex_lock (MEMC_MUTEX_PTR (mutex_)) == 0)
#define memc_unlock_mutex(mutex_)               (pthread_mutex_unlock (MEMC_MUTEX_PTR (mutex_)) == 0)

#define memc_lock_thread()        \
{                                 \
  if (!memc_lock_mutex(mutex)) {  \
    memc_abort ();                \
  }                               \
}

#define memc_unlock_thread()        \
{                                   \
  if (!memc_unlock_mutex(mutex)) {  \
    memc_abort ();                  \
  }                                 \
}

#define memc_abort()                                \
do {                                                \
  (void) fprintf (stderr,                           \
                  "[MEMC ERROR]: errno(%d): %s\n",  \
                  errno,                            \
                  strerror (errno));                \
  (void) fflush (stderr);                           \
} while (0)

#define memc_return_on_error(ec_, retval_)  \
{                                           \
  memc_error_set (ec_);                     \
  return retval_;                           \
}

#define memc_assert(expr_)                                      \
{                                                               \
  if (!(expr_)) {                                               \
    (void) fprintf (stderr, "Assertion (%s) failed\n", #expr_); \
    (void) fflush (stderr);                                     \
    memc_abort ();                                              \
  }                                                             \
}

typedef uint8_t memc_byte;
typedef uintptr_t memc_offset;
typedef size_t memc_size;

typedef enum memc_return_code memc_return_code;
typedef enum memc_data_alignment memc_data_alignment;

enum memc_return_code
{
  MEMC_RC_OK = 0x00U,
  MEMC_RC_FAILED = 0x01U
};

enum memc_data_alignment
{
  memc_alignment_fundamental = (uint32_t) (2 * MEMC_VOID_PTR_SIZE),
  memc_alignment_extended = (uint32_t) (4 * MEMC_VOID_PTR_SIZE)
};