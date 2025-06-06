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
    int level = 0;
    int completion = 0;
    int turnaround = 0;
    int waiting = 0;
    int last_executed = 0;
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
            p["pid"], p["arrival"], p["burst"], p["burst"]
        });
    }

    int time = 0, completed = 0, n = processes.size();
    const int quantum0 = 4;
    const int quantum1 = 8;

    std::vector<std::string> gantt;
    std::queue<int> q0, q1, q2;
    std::vector<bool> added(n, false);

    auto addToQueue = [&](int t) {
        for (int i = 0; i < n; ++i) {
            if (!added[i] && processes[i].arrival <= t) {
                q0.push(i);
                added[i] = true;
            }
        }
    };

    addToQueue(time);

    while (completed < n) {
        int idx = -1, exec_time = 0;

        if (!q0.empty()) {
            idx = q0.front(); q0.pop();
            exec_time = std::min(quantum0, processes[idx].remaining);
        } else if (!q1.empty()) {
            idx = q1.front(); q1.pop();
            exec_time = std::min(quantum1, processes[idx].remaining);
        } else if (!q2.empty()) {
            idx = q2.front(); q2.pop();
            exec_time = processes[idx].remaining;
        } else {
            gantt.push_back("idle");
            time++;
            addToQueue(time);
            continue;
        }

        for (int t = 0; t < exec_time; ++t) {
            gantt.push_back(processes[idx].pid);
            time++;
            addToQueue(time);
        }

        processes[idx].remaining -= exec_time;

        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            processes[idx].done = true;
            completed++;
        } else {
            processes[idx].level++;
            if (processes[idx].level == 1)
                q1.push(idx);
            else
                q2.push(idx);
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

    std::cout << "✅ Multilevel Feedback Queue scheduling complete. Output saved to output.json\n";
    return 0;
}
