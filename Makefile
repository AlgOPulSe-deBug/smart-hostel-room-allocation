CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
TARGET = hostel
SRC = $(wildcard src/*.c)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all run clean
