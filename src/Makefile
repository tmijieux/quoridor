CFLAGS=-std=c99 -Wall -g -DUSE_NCURSES -DPRINT_IN_FILE
LIBS=-lncurses
TARGET_DIR=../bin
TARGET=$(TARGET_DIR)/quoridor
# List of source files
SRC = \
	main.c \
	interface.c \
	$(wildcard strategies/*.c) \
	display.c \
# List of object files
OBJ = $(patsubst %.c, %.o, $(SRC)) 
# Command for stripping symbols
ifeq  ($(shell uname -s), Darwin)
  OBJCOPY = $(LD) -r -unexported_symbol '*' -o
else
  OBJCOPY = objcopy --strip-unneeded
endif

all: $(TARGET)

main.o: main.c 
	$(CC) -c $< $(CFLAGS) -DEXTERN_STRAT= -o $@

interface.o: interface.c interface.h 
	$(CC) -c $< $(CFLAGS) -o $@

strategies/%.o: strategies/%.c
	$(CC) -c $< -I. $(CFLAGS) -o $@

#	$(OBJCOPY) $@ $@

test:
	make -C ../tst

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)

dijk_debug: CFLAGS+=-DDIJK_DEBUG
dijk_debug: all

clean:
	$(RM) *.o strategies/*.o *~

mrproper: clean
	$(RM) $(TARGET)
