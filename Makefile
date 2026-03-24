CXX := g++-12

ifdef RUNTIME_MODE
    CXXFLAGS := -std=c++20 -g -Wall -Wextra -I. -fconstexpr-ops-limit=2147483647 -fconstexpr-loop-limit=2147483647
else
    CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -I. -fconstexpr-ops-limit=2147483648 -fconstexpr-loop-limit=2147483647
endif

COMPILE_FLAGS := -c $(CXXFLAGS)

ifdef RUNTIME_MODE
	CXXFLAGS += -DRUNTIME_MODE
endif

TARGET := ctwr
OBJ := main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cpp parser.hpp state.hpp types.hpp constants.hpp handler.hpp runner.hpp inspect.hpp test/program.hpp parsedState.hpp syscall.hpp
	$(CXX) $(COMPILE_FLAGS) main.cpp -o main.o

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean