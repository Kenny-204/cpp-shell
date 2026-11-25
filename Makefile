CXX := c++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic
TARGET := cpp-shell
SOURCES := Project1.cpp cd.cpp execute.cpp inputParser.cpp pipes.cpp redirections.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
