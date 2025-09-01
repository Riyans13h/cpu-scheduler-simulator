#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;

struct Process {
    std::string pid;
    int arrival;
    int burst;
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;

    bool operator>(const Process& other) const {
        return burst > other.burst; // for priority_queue (min-heap)
    }
};

// Comparator for priority queue
struct CompareBurst {
    bool operator()(const Process& a, const Process& b) {
        return a.burst > b.burst;
    }
};

int main() {
    std::ifstream inFile("../data/input.json");
    if (!inFile) {
        std::cerr << "Error: Cannot open input.json\n";
        return 1;
    }

    json input;
    try {
        inFile >> input;
    } catch (std::exception &e) {
        std::cerr << "JSON Error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<Process> processes;
    for (const auto& p : input["processes"]) {
        processes.push_back({p["pid"], p["arrival"], p["burst"]});
    }

    // Step 1: Sort by arrival time
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    int n = processes.size();
    int time = 0;
    int completed = 0;
    int arrival_ptr = 0;
    std::vector<std::string> gantt;

    std::priority_queue<Process, std::vector<Process>, CompareBurst> pq;

    while (completed < n) {
        // Step 2: Push all processes that have arrived by current time
        while (arrival_ptr < n && processes[arrival_ptr].arrival <= time) {
            pq.push(processes[arrival_ptr]);
            arrival_ptr++;
        }

        // Step 3: Execute next shortest job if available
        if (!pq.empty()) {
            Process current = pq.top(); pq.pop();

            for (int t = 0; t < current.burst; ++t)
                gantt.push_back(current.pid);

            time += current.burst;
            current.completion = time;
            current.turnaround = current.completion - current.arrival;
            current.waiting = current.turnaround - current.burst;

            // Update original process info
            for (auto &p : processes) {
                if (p.pid == current.pid && p.arrival == current.arrival) {
                    p.completion = current.completion;
                    p.turnaround = current.turnaround;
                    p.waiting = current.waiting;
                    break;
                }
            }

            completed++;
        } else {
            // If no process is ready, CPU is idle
            gantt.push_back("idle");
            time++;
        }
    }

    // Step 4: Prepare output JSON
    json output;
    output["gantt_chart"] = gantt;

    double total_waiting = 0, total_turnaround = 0;
    for (const auto &p : processes) {
        output["results"].push_back({
            {"pid", p.pid},
            {"arrival", p.arrival},
            {"burst", p.burst},
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

    std::cout << "Optimized SJF (non-preemptive) complete. Output saved to output.json\n";
    return 0;
}
