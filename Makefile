# Get git version 
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# Console colors and style definition
GREEN = "\\033[32m"
YELLOW = "\\033[33m"
CYAN = "\\033[36m"
NORMAL = "\\033[0m"
BOLD = "\\033[1m"

# Sources
SOURCES := $(wildcard *.cpp)

# Objects
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

# Compiler
CXX ?= g++

# Compiler flags
CXXFLAGS = -Wall -Werror -DVERSION=\"$(GIT_VERSION)\"

# Target applciation
TARGET := custom_desktop_controll

# Makefile routines
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo -e "Linking $(GREEN)$^$(NORMAL) -> $(BOLD)$(YELLOW)$(TARGET)$(NORMAL)"
	@$(CXX) $^ -o $(TARGET)

%.o: %.cpp
	@echo -e "Compiling $(CYAN)$<$(NORMAL) -> $(BOLD)$(GREEN)$@$(NORMAL)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning"
	@rm -rf $(TARGET) $(OBJECTS)
