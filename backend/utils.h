#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "data_structures.h"

// Parses input from a JSON file and returns a vector of Process structs
std::vector<Process> parseInputJSON(const std::string &filename);

// Writes output JSON with Gantt chart and average times
void writeOutputJSON(const std::string &filename,
                     const std::vector<Result> &results,
                     float avg_waiting_time,
                     float avg_turnaround_time);

#endif  // UTILS_H
