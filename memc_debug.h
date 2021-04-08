/** Package: memc
 ** Module:  debug (debug mode routines)
 ** Version: 0.1
 */

#if defined(DEBUG)

typedef enum
{
  memc_event_block_created = 0x00U,
  memc_event_block_disposed = 0x01U
} memc_debug_event;

#define memc_debug_trace_stream stderr

#define memc_debug_trace_fmt(fmt, ...)          \
do {                                            \
    (void) fprintf (memc_debug_trace_stream,    \
                    "%s:%d: " fmt "\n",         \
                    __FILE__,                   \
                    __LINE__,                   \
                    __VA_ARGS__);               \
   (void) fflush (memc_debug_trace_stream);     \
} while (0)


#define memc_debug_trace_event(event)      memc_debug_trace_fmt ("[%s]", #event)

#define memc_debug_trace_region(region_)   memc_debug_trace_fmt ("alignment: %d pagesize: %d location: %s blocks: %d",  \
                                                MEMC_INT32 ((region_)->alignment),                                      \
                                                MEMC_INT32 ((region_)->page_size),                                      \
                                                MEMC_BIT_IS_SET ((region_)->location, memc_region_location_heap)        \
                                                ? "heap"                                                                \
                                                : "mmap",                                                               \
                                                MEMC_INT32 ((region_)->block_count));
#else

#define memc_debug_trace_stream
#define memc_debug_trace_fmt(fmt, ...)    do {} while (0)
#define memc_debug_trace_event(event)     do {} while (0)
#define memc_debug_trace_region(region_)  do {} while (0)

#endif
