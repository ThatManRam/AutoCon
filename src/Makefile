CC = gcc
CFLAGS = -Wall -Wextra -g $(shell pkg-config --cflags libxml-2.0)
LDFLAGS = $(shell pkg-config --libs libxml-2.0)

TARGET = network_mapper

SRC = main.c network.c nmap.c parser.c dns.c output.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf output