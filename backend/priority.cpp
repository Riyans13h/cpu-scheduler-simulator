#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
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
    bool added = false; // to prevent re-adding
};

// Custom comparator for max-heap: higher priority first, if equal then earlier arrival
struct Compare {
    bool operator()(const Process* a, const Process* b) const {
        if (a->priority == b->priority)
            return a->arrival > b->arrival;  // earlier arrival preferred
        return a->priority > b->priority;    // smaller number = higher priority
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
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<Process> processes;
    for (auto &p : input["processes"]) {
        processes.push_back({
            p["pid"], p["arrival"], p["burst"], p["priority"]
        });
    }

    // Sort processes by arrival time for easier handling
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    std::priority_queue<Process*, std::vector<Process*>, Compare> pq;

    int time = 0, completed = 0, n = processes.size(), i = 0;
    std::vector<std::string> gantt;

    while (completed < n) {
        // Add all processes that have arrived at current time
        while (i < n && processes[i].arrival <= time) {
            if (!processes[i].added) {
                pq.push(&processes[i]);
                processes[i].added = true;
            }
            ++i;
        }

        if (!pq.empty()) {
            Process* current = pq.top(); pq.pop();
            for (int t = 0; t < current->burst; ++t) {
                gantt.push_back(current->pid);
            }

            time += current->burst;
            current->completion = time;
            current->turnaround = current->completion - current->arrival;
            current->waiting = current->turnaround - current->burst;
            current->done = true;
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

    std::cout << "Priority scheduling (using max-heap) complete. Output saved to output.json\n";
    return 0;
}
