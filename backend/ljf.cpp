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
    bool added = false;
    bool done = false;
};

// Max-heap: longest burst first
struct CompareBurst {
    bool operator()(const Process* a, const Process* b) const {
        if (a->burst == b->burst)
            return a->arrival > b->arrival; // Earlier arrival wins tie
        return a->burst < b->burst;         // Longer burst wins
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
    for (const auto &p : input["processes"]) {
        processes.push_back({p["pid"], p["arrival"], p["burst"]});
    }

    // ✅ Sort by arrival time
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    int n = processes.size();
    int time = 0, completed = 0;
    std::vector<std::string> gantt;

    std::priority_queue<Process*, std::vector<Process*>, CompareBurst> pq;

    while (completed < n) {
        for (auto &p : processes) {
            if (p.arrival <= time && !p.added) {
                pq.push(&p);
                p.added = true;
            }
        }

        if (!pq.empty()) {
            Process* current = pq.top();
            pq.pop();

            // Execute process fully (non-preemptive)
            for (int t = 0; t < current->burst; ++t)
                gantt.push_back(current->pid);

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

    // Prepare output
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

    std::cout << "LJF scheduling complete with arrival time sorting. Output saved to output.json\n";
    return 0;
}
