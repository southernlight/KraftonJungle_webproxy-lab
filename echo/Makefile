# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executables
TARGETS = echoclient echoserver

# Object files
OBJS = csapp.o

# Compile targets
all: $(TARGETS)

# echoclient build rules
echoclient: echoclient.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ echoclient.o $(OBJS)

# echoserver build rules
echoserver: echoserver.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ echoserver.o $(OBJS)

# csapp.c compilation rule
csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

# Source file compilation rules
echoclient.o: echoclient.c csapp.h
	$(CC) $(CFLAGS) -c echoclient.c

echoserver.o: echoserver.c csapp.h
	$(CC) $(CFLAGS) -c echoserver.c

# Clean rule
clean:
	rm -f $(TARGETS) *.o
