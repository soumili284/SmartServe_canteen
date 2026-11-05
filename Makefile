CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = bin/SmartServe.exe
SRCDIR = src
INCDIR = include
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o, $(SOURCES))

all: directories $(TARGET)

directories:
	@mkdir -p $(BINDIR)
	@mkdir -p data
	@mkdir -p receipts

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)/*.o $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run directories
