CC = gcc
OUT_FILE_NAME = libmemc.a
DEBUG_MODE=1
ifeq ($(DEBUG_MODE), 1)
DEBUG_FLAGS= -g -DDEBUG=1 -O0
else
DEBUG_FLAGS= -O3
endif

COMPILER_FLAGS= -c -std=c99 -D_DEFAULT_SOURCE=1 -D_BSD_SOURCE=1 -D_XOPEN_SOURCE=600 -D_REENTRANT=1 -Wall -Wfatal-errors
INC = -I.
OBJ_DIR=./bin
OUT_DIR=./bin
FILES = memc_error.c memc_heap_1.c memc_heap_2.c memc_heap_3.c memc_mmap.c memc_region.c

$(OUT_FILE_NAME): $(patsubst %.c,$(OBJ_DIR)/%.o,$(wildcard $(FILES))) 
	ar -r -o $(OUT_DIR)/$@ $^

$(OBJ_DIR)/%.o: %.c dirmake
	$(CC) -c $(INC) $(DEBUG_FLAGS) $(COMPILER_FLAGS) -o $@  $< -lpthread
	
dirmake:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak

rebuild: clean build 
