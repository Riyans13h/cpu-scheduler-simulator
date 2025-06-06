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
    int queue; // 1 = high priority (RR), 2 = low priority (FCFS)
    int remaining;
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
            p["pid"], p["arrival"], p["burst"], p["queue"], p["burst"]
        });
    }

    int time = 0, completed = 0, n = processes.size();
    const int quantum = 2;
    std::vector<std::string> gantt;

    std::queue<int> q1, q2;
    std::vector<bool> added(n, false);

    while (completed < n) {
        // Add new arrivals to queues
        for (int i = 0; i < n; ++i) {
            if (!added[i] && processes[i].arrival <= time) {
                if (processes[i].queue == 1) q1.push(i);
                else q2.push(i);
                added[i] = true;
            }
        }

        if (!q1.empty()) {
            int idx = q1.front(); q1.pop();
            int exec = std::min(quantum, processes[idx].remaining);

            for (int t = 0; t < exec; ++t) {
                gantt.push_back(processes[idx].pid);
                time++;

                // Add any new arrivals during execution
                for (int i = 0; i < n; ++i) {
                    if (!added[i] && processes[i].arrival <= time) {
                        if (processes[i].queue == 1) q1.push(i);
                        else q2.push(i);
                        added[i] = true;
                    }
                }
            }

            processes[idx].remaining -= exec;

            if (processes[idx].remaining > 0)
                q1.push(idx); // Not finished, requeue
            else {
                processes[idx].completion = time;
                processes[idx].turnaround = time - processes[idx].arrival;
                processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
                processes[idx].done = true;
                completed++;
            }

        } else if (!q2.empty()) {
            int idx = q2.front(); q2.pop();
            for (int t = 0; t < processes[idx].remaining; ++t) {
                gantt.push_back(processes[idx].pid);
                time++;

                // Add arrivals
                for (int i = 0; i < n; ++i) {
                    if (!added[i] && processes[i].arrival <= time) {
                        if (processes[i].queue == 1) q1.push(i);
                        else q2.push(i);
                        added[i] = true;
                    }
                }
            }

            processes[idx].remaining = 0;
            processes[idx].completion = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            processes[idx].done = true;
            completed++;

        } else {
            gantt.push_back("idle");
            time++;
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
            {"queue", p.queue},
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

    std::cout << "✅ Multiple Queue scheduling complete. Output saved to output.json\n";
    return 0;
}
