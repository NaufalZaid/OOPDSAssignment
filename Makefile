# Define the compiler
CXX = g++

# Define the compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -I.

# Define the source files
SRCS = src/main.cpp \
       src/GameManager.cpp \
       src/ship.cpp \
       src/shipTypes.cpp \
       src/parseFile.cpp \
       src/shipTypes/Amphibious.cpp \
       src/shipTypes/Battleship.cpp \
       src/shipTypes/Corvette.cpp \
       src/shipTypes/Cruiser.cpp \
       src/shipTypes/Destroyer.cpp \
       src/shipTypes/Frigate.cpp \
       src/shipTypes/SuperShip.cpp \
       src/Queue.cpp

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Define the executable name
EXEC = game

# Default target
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJS)
	$(CXX)	$(CXXFLAGS)	-o	$@	$^

# Compile the source files into object files
%.o: %.cpp
	$(CXX)	$(CXXFLAGS)	-c	$<	-o	$@

# Clean up the build files
clean:
	rm	-f	$(OBJS)	$(EXEC)

# Phony targets
.PHONY:`all`clean