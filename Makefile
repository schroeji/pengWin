# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = main

#includes
INC=-I/usr/include/python3.6m -lpython3.6m

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) memory_access.cpp manager.cpp visu.cpp $(TARGET).cpp

clean:
	$(RM) $(TARGET)
