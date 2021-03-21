# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -MD -std=c++17

# the build target executable:
TARGET = pengWin
TARGET_DUMPER = offset_dumper

#includes
INC=-lX11 -lXtst -lboost_thread -lboost_system -pthread -I src/

SRC=$(wildcard src/main.cpp src/misc/*.cpp src/hacks/*.cpp src/ValveBSP/*.cpp)
SRC_DUMPER = src/offset_dumper/offset_finder.cpp src/offset_dumper/netvar_finder.cpp src/misc/util.cpp src/misc/memory_access.cpp

all: $(TARGET) $(TARGET_DUMPER)
all: CFLAGS += -O3
debug: CFLAGS += -DDEBUG -g
debug: $(TARGET) $(TARGET_DUMPER)
pengwin: $(TARGET)
pengwin: CFLAGS += -O3
offset: $(TARGET_DUMPER)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SRC)

$(TARGET_DUMPER): $(SRC_DUMPER)
	$(CC) $(CFLAGS) $(INC) -o $(TARGET_DUMPER) $(SRC_DUMPER)

clean:
	$(RM) $(TARGET) $(TARGET_DUMPER)
