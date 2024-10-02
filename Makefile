CC = gcc

CFLAGS = -g

EXEC = disassem

SRCS = parser.c interpret_instruction.c

HEADERS = interpret_instruction.h

OBJS = $(SRCS:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

parser.o: parser.c interpret_instruction.h $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c 

interpret_instructions.o: interpret_instruction.c interpret_instruction.h $(HEADERS)
	$(CC) $(CFLAGS) -c interpret_instructions.c 

clean:
	rm -f $(EXEC) $(OBJS)