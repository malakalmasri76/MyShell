
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = myshell
OBJS = myshell.o utility.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

utility.o: utility.c myshell.h
	$(CC) $(CFLAGS) -c utility.c

clean:
	rm -f $(OBJS) $(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean debug
