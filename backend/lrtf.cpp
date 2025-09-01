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
    bool finished = false;
    bool added = false;
};

// Comparator for max-heap (Longest Remaining Time First)
struct CompareRemaining {
    bool operator()(const Process* a, const Process* b) const {
        if (a->remaining == b->remaining)
            return a->arrival > b->arrival; // Earlier arrival preferred
        return a->remaining < b->remaining; // Max-heap (longer remaining first)
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
        int burst = p["burst"];
        processes.push_back({p["pid"], p["arrival"], burst, burst});
    }

    // Sort processes by arrival for consistency
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    int time = 0;
    int completed = 0;
    int n = processes.size();
    std::vector<std::string> gantt;

    std::priority_queue<Process*, std::vector<Process*>, CompareRemaining> pq;

    while (completed < n) {
        // Push newly arrived processes
        for (auto &p : processes) {
            if (p.arrival <= time && !p.added) {
                pq.push(&p);
                p.added = true;
            }
        }

        if (!pq.empty()) {
            Process* current = pq.top();
            pq.pop();

            gantt.push_back(current->pid);
            current->remaining--;

            // Re-add to queue if not finished
            if (current->remaining > 0) {
                pq.push(current);
            } else {
                current->completion = time + 1;
                current->turnaround = current->completion - current->arrival;
                current->waiting = current->turnaround - current->burst;
                current->finished = true;
                completed++;
            }
        } else {
            gantt.push_back("idle");
        }

        time++;
    }

    // Output generation
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

    std::cout << "LRTF (preemptive) scheduling complete. Output saved to output.json\n";
    return 0;
}
