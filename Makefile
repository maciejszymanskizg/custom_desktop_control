SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

CXX ?= g++
CXXFLAGS = -Wall -Werror

TARGET := app

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJECTS)
