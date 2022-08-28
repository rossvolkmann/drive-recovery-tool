CC = gcc
CFLAGS = -Wall -Wextra -Werror
DFLAGS = -g
DEPENDENCIES.C = read_ext2.c
EXEC = runscan
MAIN.C = runscan.c

default: main

clean:
	rm -f $(EXEC)

main: $(MAIN.C)
	$(CC) $(CFLAGS) $(DFLAGS) $(MAIN.C) $(DEPENDENCIES.C) -o $(EXEC)
