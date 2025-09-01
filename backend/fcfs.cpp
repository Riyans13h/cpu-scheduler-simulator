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
        return burst > other.burst; // For min-heap behavior
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
    for (auto &p : input["processes"]) {
        processes.push_back({p["pid"], p["arrival"], p["burst"]});
    }

    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    int time = 0, completed = 0, n = processes.size(), arrival_ptr = 0;
    std::vector<std::string> gantt;
    std::priority_queue<Process, std::vector<Process>, CompareBurst> pq;

    while (completed < n) {
        // Push all processes that have arrived by 'time' into the queue
        while (arrival_ptr < n && processes[arrival_ptr].arrival <= time) {
            pq.push(processes[arrival_ptr]);
            arrival_ptr++;
        }

        if (!pq.empty()) {
            Process current = pq.top(); pq.pop();

            for (int i = 0; i < current.burst; ++i)
                gantt.push_back(current.pid);

            time += current.burst;
            current.completion = time;
            current.turnaround = current.completion - current.arrival;
            current.waiting = current.turnaround - current.burst;

            // Update the original process (match by pid and arrival)
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
            gantt.push_back("idle");
            time++;
        }
    }

    // Output JSON
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

    std::cout << "SJF scheduling complete. Output saved to output.json\n";
    return 0;
}
