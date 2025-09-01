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
    bool inQueue = false;
};

// Sort processes based on arrival time (and optionally PID for stability)
bool arrivalSort(const Process& a, const Process& b) {
    if (a.arrival == b.arrival)
        return a.pid < b.pid;
    return a.arrival < b.arrival;
}

int main() {
    std::ifstream inFile("../data/input.json");
    if (!inFile) {
        std::cerr << "Error: Cannot open input.json\n";
        return 1;
    }

    json input;
    try {
        inFile >> input;
    } catch (std::exception& e) {
        std::cerr << "JSON Error: " << e.what() << std::endl;
        return 1;
    }

    int quantum = input.value("quantum", 2);
    std::vector<Process> processes;

    for (auto& p : input["processes"]) {
        int burst = p["burst"];
        processes.push_back({p["pid"], p["arrival"], burst, burst});
    }

    // ✅ Step 1: Sort by arrival time
    std::sort(processes.begin(), processes.end(), arrivalSort);

    int n = processes.size();
    int time = 0, completed = 0;
    std::queue<int> readyQueue;
    std::vector<std::string> gantt;

    while (completed < n) {
        // Step 2: Add all newly arrived processes to queue
        for (int i = 0; i < n; ++i) {
            if (!processes[i].inQueue && processes[i].arrival <= time && processes[i].remaining > 0) {
                readyQueue.push(i);
                processes[i].inQueue = true;
            }
        }

        // Step 3: If no process is ready, CPU is idle
        if (readyQueue.empty()) {
            gantt.push_back("idle");
            time++;
            continue;
        }

        // Step 4: Fetch the next process in the queue
        int idx = readyQueue.front();
        readyQueue.pop();

        int execTime = std::min(quantum, processes[idx].remaining);
        for (int t = 0; t < execTime; ++t) {
            gantt.push_back(processes[idx].pid);
        }

        time += execTime;
        processes[idx].remaining -= execTime;

        // Step 5: Add new arrivals during this execution
        for (int i = 0; i < n; ++i) {
            if (!processes[i].inQueue && processes[i].arrival <= time && processes[i].remaining > 0) {
                readyQueue.push(i);
                processes[i].inQueue = true;
            }
        }

        // Step 6: Requeue if not completed
        if (processes[idx].remaining > 0) {
            readyQueue.push(idx);
        } else {
            processes[idx].completion = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            completed++;
            processes[idx].inQueue = false;
        }
    }

    // Prepare JSON output
    json output;
    output["gantt_chart"] = gantt;

    double total_waiting = 0, total_turnaround = 0;
    for (const auto& p : processes) {
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

    std::cout << "✅ Round Robin scheduling complete. Output saved to output.json\n";
    return 0;
}
