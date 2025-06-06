#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;

struct Process {
    std::string pid;
    int arrival;
    int burst;
    int priority;
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;
    bool done = false;
};

int main() {
    std::ifstream inFile("../data/input.json");
    if (!inFile) {
        std::cerr << "❌ Error: Cannot open input.json\n";
        return 1;
    }

    json input;
    try {
        inFile >> input;
    } catch (std::exception &e) {
        std::cerr << "❌ JSON Parse Error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<Process> processes;
    for (auto &p : input["processes"]) {
        processes.push_back({
            p["pid"], p["arrival"], p["burst"], p["priority"]
        });
    }

    int time = 0, completed = 0, n = processes.size();
    std::vector<std::string> gantt;

    while (completed < n) {
        int idx = -1;
        int highestPriority = INT_MAX;

        for (int i = 0; i < n; ++i) {
            if (!processes[i].done && processes[i].arrival <= time && processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            gantt.push_back("idle");
            time++;
            continue;
        }

        Process &p = processes[idx];
        for (int t = 0; t < p.burst; ++t) gantt.push_back(p.pid);

        time += p.burst;
        p.completion = time;
        p.turnaround = p.completion - p.arrival;
        p.waiting = p.turnaround - p.burst;
        p.done = true;
        completed++;
    }

    // Output
    json output;
    output["gantt_chart"] = gantt;

    double total_waiting = 0, total_turnaround = 0;
    for (const auto &p : processes) {
        output["results"].push_back({
            {"pid", p.pid},
            {"arrival", p.arrival},
            {"burst", p.burst},
            {"priority", p.priority},
            {"completion", p.completion},
            {"turnaround", p.turnaround},
            {"waiting", p.waiting}
        });
        total_waiting += p.waiting;
        total_turnaround += p.turnaround;
    }

    output["average_waiting"] = total_waiting / n;
    output["average_turnaround"] = total_turnaround / n;

    std::ofstream outFile("../data/output.json");
    outFile << output.dump(4);

    std::cout << "✅ Priority scheduling complete. Output saved to output.json\n";
    return 0;
}
