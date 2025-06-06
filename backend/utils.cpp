#include "utils.h"
#include "data_structures.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

std::vector<Process> parseInputJSON(const std::string &filename) {
    std::vector<Process> processes;
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        throw std::runtime_error("Error: Cannot open input file: " + filename);
    }

    json input;
    input_file >> input;

    for (const auto& p : input["processes"]) {
        Process proc;
        proc.pid = p["pid"];
        proc.arrival_time = p["arrival_time"];
        proc.burst_time = p["burst_time"];
        proc.priority = p["priority"];
        processes.push_back(proc);
    }

    return processes;
}

void writeOutputJSON(const std::string &filename,
                     const std::vector<Result> &results,
                     float avg_waiting_time,
                     float avg_turnaround_time) {
    json output;
    output["gantt_chart"] = json::array();

    for (const auto& r : results) {
        json entry = {
            {"pid", r.pid},
            {"start_time", r.start_time},
            {"end_time", r.end_time}
        };
        output["gantt_chart"].push_back(entry);
    }

    output["average_waiting_time"] = avg_waiting_time;
    output["average_turnaround_time"] = avg_turnaround_time;

    std::ofstream output_file(filename);
    if (!output_file.is_open()) {
        throw std::runtime_error("Error: Cannot write to output file: " + filename);
    }

    output_file << output.dump(4);  // Pretty print with indentation
}
