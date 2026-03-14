# ----------------------
# Makefile for ctwr
# ----------------------

CXX := g++
CXXFLAGS := -c -std=c++20 -O2 -Wall -Wextra -I. 
LDFLAGS := 

TARGET := ctwr
SRC := main.cpp parser.hpp state.hpp types.hpp constants.hpp
OBJ := main.o

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile main.cpp to object
main.o: main.cpp parser.hpp state.hpp types.hpp constants.hpp handler.hpp runner.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Clean object files and executable
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean