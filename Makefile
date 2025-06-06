# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Source files
SOURCES = fcfs.cpp sjf.cpp srtf.cpp rr.cpp priority.cpp hrrn.cpp mqs.cpp ljf.cpp lrjf.cpp mlfq.cpp
OBJECTS = $(SOURCES:.cpp=)

# Default target: build all
all: $(OBJECTS)

# Pattern rule to build each executable
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Clean up compiled binaries and output.json
clean:
	rm -f $(OBJECTS) ../data/output.json
