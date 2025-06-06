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
        std::cerr << "❌ JSON Error: " << e.what() << std::endl;
        return 1;
    }

    int quantum = input["time_quantum"];
    std::vector<Process> processes;
    for (auto &p : input["processes"]) {
        int burst = p["burst"];
        processes.push_back({p["pid"], p["arrival"], burst, burst});
    }

    std::queue<int> readyQueue;
    int time = 0, completed = 0, n = processes.size();
    std::vector<std::string> gantt;
    std::vector<bool> inQueue(n, false);

    while (completed < n) {
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrival <= time && !inQueue[i] && processes[i].remaining > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        if (readyQueue.empty()) {
            gantt.push_back("idle");
            time++;
            continue;
        }

        int idx = readyQueue.front();
        readyQueue.pop();

        int execTime = std::min(quantum, processes[idx].remaining);
        for (int t = 0; t < execTime; ++t) {
            gantt.push_back(processes[idx].pid);
        }

        time += execTime;
        processes[idx].remaining -= execTime;

        // Check if completed
        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            completed++;
        }

        // Re-add to queue if still remaining
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrival <= time && !inQueue[i] && processes[i].remaining > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        if (processes[idx].remaining > 0) {
            readyQueue.push(idx);
        } else {
            inQueue[idx] = false;
        }
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
