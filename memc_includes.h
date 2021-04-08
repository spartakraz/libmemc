#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#include "memc_debug.h"
#include "memc_defs.h"
#include "memc_inline.h"
#include "memc_error.h"
#include "memc_heap.h"
#include "memc_mmap.h"