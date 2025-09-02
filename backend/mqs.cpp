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
    int priority; // 1 = high, 2 = low
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;
};

// Comparator for priority queue
struct Compare {
    bool operator()(const Process* a, const Process* b) {
        if (a->priority != b->priority)
            return a->priority > b->priority; // lower number = higher priority
        return a->arrival > b->arrival;       // earlier arrival first if same priority
    }
};

int main() {
    std::ifstream inFile("../data/input.json");
    if (!inFile) {
        std::cerr << "❌ Cannot open input.json\n";
        return 1;
    }

    json input;
    inFile >> input;

    int quantum = input.value("quantum", 2); // For high-priority RR

    std::vector<Process> processes;
    for (auto &p : input["processes"]) {
        int burst = p["burst"];
        processes.push_back({p["pid"], p["arrival"], burst, burst, p["queue"]});
    }

    auto cmp = Compare();
    std::priority_queue<Process*, std::vector<Process*>, Compare> pq;
    int time = 0, completed = 0, n = processes.size();
    std::vector<std::string> gantt;
    std::vector<bool> inQueue(n, false);

    while (completed < n) {
        // Add newly arrived processes
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrival <= time && !inQueue[i] && processes[i].remaining > 0) {
                pq.push(&processes[i]);
                inQueue[i] = true;
            }
        }

        if (pq.empty()) {
            gantt.push_back("idle");
            time++;
            continue;
        }

        Process* p = pq.top();
        pq.pop();

        if (p->priority == 1) { // High priority -> Round Robin
            int execTime = std::min(quantum, p->remaining);
            for (int t = 0; t < execTime; ++t) {
                gantt.push_back(p->pid);
                time++;

                // Add newly arrived processes during execution
                for (int i = 0; i < n; ++i) {
                    if (processes[i].arrival <= time && !inQueue[i] && processes[i].remaining > 0) {
                        pq.push(&processes[i]);
                        inQueue[i] = true;
                    }
                }
            }

            p->remaining -= execTime;
            if (p->remaining > 0) pq.push(p);
            else {
                p->completion = time;
                p->turnaround = p->completion - p->arrival;
                p->waiting = p->turnaround - p->burst;
                completed++;
            }

        } else { // Low priority -> FCFS
            for (int t = 0; t < p->remaining; ++t) {
                gantt.push_back(p->pid);
                time++;

                // Add newly arrived processes
                for (int i = 0; i < n; ++i) {
                    if (processes[i].arrival <= time && !inQueue[i] && processes[i].remaining > 0) {
                        pq.push(&processes[i]);
                        inQueue[i] = true;
                    }
                }
            }

            p->remaining = 0;
            p->completion = time;
            p->turnaround = p->completion - p->arrival;
            p->waiting = p->turnaround - p->burst;
            completed++;
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
            {"queue", p.priority},
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

    std::cout << "✅ Multi-Level Queue (priority queue) scheduling complete.\n";
    return 0;
}
