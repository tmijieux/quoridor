CFLAGS=-std=c99 -Wall -g -DTEST
LIBS=-lncurses
TARGET_DIR=../bin
TARGET=$(TARGET_DIR)/test_quoridor
# List of source files
SRC = 	../src/interface.c \
	test_interface.c \
	../src/display.c
# List of object files
OBJ = $(patsubst %.c, %.o, $(SRC)) 

all: $(TARGET) test_random_strategy

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) *~ *.o

mrproper: clean
	$(RM) $(TARGET) test_random_strategy

test_random_strategy.o: test_random_strategy.c
	$(CC) $(CFLAGS) -c -I../src  -DUSE_NCURSES  -DEXTERN_STRAT= -o $@ $^ 

test_random_strategy: ../src/interface.c ../src/display.c test_random_strategy.o ../src/strategies/strategy1.c init_board_debug.c
	$(CC) $(CFLAGS) -DUSE_NCURSES -I../src -o $@ $^ $(LIBS)
