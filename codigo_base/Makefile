CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic-errors
TARGET   := tp02
SOURCES  := main.cpp BenchmarkRunner.cpp HashExternal.cpp HashInternal.cpp
HEADERS  := HashCommon.h BenchmarkRunner.h HashExternal.h HashInternal.h

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET) 1000 2000

ifeq ($(OS),Windows_NT)
    CLEAN_CMD = cmd /C "del /Q /F $(TARGET) $(TARGET).exe *.o *.bin *.gch 2>nul || exit 0"
else
    CLEAN_CMD = rm -f $(TARGET) $(TARGET).exe *.o *.bin *.gch
endif

clean:
	$(CLEAN_CMD)
