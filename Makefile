# Compiler and flags
CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -Iinclude 

# Name of the final executable
TARGET    = warship_sim

# List of source files in src/
SRCS = \
	src/Queue.cpp \
	src/Battlefield.cpp \
	src/GameManager.cpp \
	src/Ship.cpp \
	src/ShipTypes.cpp \
	src/parseFile.cpp \
	src/SeeingRobot.cpp \
	src/main.cpp

# Object files (replace .cpp with .o)
OBJS = $(SRCS:.cpp=.o)

# Default rule: build the final executable
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compiling each .cpp into .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
