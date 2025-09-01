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
    int remaining;
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;
    bool added = false;  // To ensure a process is only added once
};

// Comparator for min-heap (shortest remaining time first)
struct CompareRemaining {
    bool operator()(const Process* a, const Process* b) const {
        if (a->remaining == b->remaining)
            return a->arrival > b->arrival; // Tie-breaker: earlier arrival first
        return a->remaining > b->remaining; // Smaller remaining time first
    }
};

int main() {
    // Step 1: Read JSON input
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

    // Step 2: Load processes
    std::vector<Process> processes;
    for (const auto &p : input["processes"]) {
        int burst = p["burst"];
        processes.push_back({p["pid"], p["arrival"], burst, burst});
    }

    int time = 0;
    int completed = 0;
    int n = processes.size();
    std::vector<std::string> gantt;

    // Priority queue to pick the process with shortest remaining time
    std::priority_queue<Process*, std::vector<Process*>, CompareRemaining> pq;

    // Step 3: Main simulation loop
    while (completed < n) {
        // Add newly arrived processes to the queue
        for (auto &p : processes) {
            if (p.arrival <= time && !p.added) {
                pq.push(&p);
                p.added = true;
            }
        }

        if (!pq.empty()) {
            Process* current = pq.top();
            pq.pop();

            // Execute 1 unit of the process
            gantt.push_back(current->pid);
            current->remaining--;

            if (current->remaining == 0) {
                current->completion = time + 1;
                current->turnaround = current->completion - current->arrival;
                current->waiting = current->turnaround - current->burst;
                completed++;
            } else {
                pq.push(current); // Reinsert with updated remaining time
            }
        } else {
            gantt.push_back("idle"); // No process available, CPU idle
        }

        time++;
    }

    // Step 4: Prepare output
    json output;
    output["gantt_chart"] = gantt;

    double total_waiting = 0;
    double total_turnaround = 0;

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

    // Step 5: Write output to file
    std::ofstream outFile("../data/output.json");
    if (!outFile) {
        std::cerr << "Error: Cannot write to output.json\n";
        return 1;
    }
    outFile << output.dump(4);

    std::cout << "SRTF scheduling complete. Output saved to output.json\n";
    return 0;
}
