# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = main

#includes
INC=-lX11

SRC=$(wildcard src/*.cpp)

all: $(TARGET)

$(TARGET): src/main.cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SRC)

clean:
	$(RM) $(TARGET)
