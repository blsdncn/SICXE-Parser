CC = gcc

CFLAGS = -g -I$(LIB_DIR)

EXEC = disassem

LIB_DIR = disassemLib

SRCS = parser.c $(LIB_DIR)/interpret_instruction.c

HEADERS = $(LIB_DIR)/interpret_instruction.h 

OBJS = parser.o $(LIB_DIR)/interpret_instruction.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

parser.o: parser.c $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c 

$(LIB_DIR)/interpret_instruction.o: $(LIB_DIR)/interpret_instruction.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(LIB_DIR)/interpret_instruction.c -o $(LIB_DIR)/interpret_instruction.o

clean:
	rm -f $(EXEC) $(OBJS)
