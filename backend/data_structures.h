#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>

// Struct to store process data
struct Process {
    std::string pid;
    int arrival_time;
    int burst_time;
    int priority;
};

// Struct to store result (for Gantt chart)
struct Result {
    std::string pid;
    int start_time;
    int end_time;
};

#endif  // DATA_STRUCTURES_H
