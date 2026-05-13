CC = gcc
CFLAGS = -Wall -g -I./include

SRC = src/main.c src/tools.c src/tsock-hosts.c src/BAL-mgmt.c src/BAL-hosts.c
OBJ = $(SRC:.c=.o)

tsock: $(OBJ)
	$(CC) $(CFLAGS) -o tsock $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) tsock
