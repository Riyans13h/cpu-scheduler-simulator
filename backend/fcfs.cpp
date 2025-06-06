// backend/fcfs.cpp

#include "scheduler.h"
#include <fstream>
#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

Output fcfs(std::vector<Process> &processes) {
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    int current_time = 0;
    std::vector<GanttSegment> gantt;
    std::vector<Result> results;
    double total_wait = 0, total_turnaround = 0;

    for (auto &p : processes) {
        if (current_time < p.arrival) {
            current_time = p.arrival;
        }
        GanttSegment segment = {p.pid, current_time, current_time + p.burst};
        gantt.push_back(segment);

        int waiting = current_time - p.arrival;
        int turnaround = waiting + p.burst;

        results.push_back({p.pid, waiting, turnaround});
        total_wait += waiting;
        total_turnaround += turnaround;

        current_time += p.burst;
    }

    return {gantt, results, total_wait / processes.size(), total_turnaround / processes.size()};
}

int main() {
    std::ifstream inFile("../data/input.json");
    json input;
    inFile >> input;

    std::vector<Process> processes;
    for (auto &p : input["processes"]) {
        processes.push_back({p["pid"], p["arrival"], p["burst"], p.value("priority", 0), p["burst"]});
    }

    Output out = fcfs(processes);

    json output_json;
    for (auto &g : out.gantt) {
        output_json["gantt_chart"].push_back({{"pid", g.pid}, {"start", g.start}, {"end", g.end}});
    }
    for (auto &r : out.results) {
        output_json["results"].push_back({{"pid", r.pid}, {"waiting", r.waiting}, {"turnaround", r.turnaround}});
    }
    output_json["average_waiting"] = out.avg_waiting;
    output_json["average_turnaround"] = out.avg_turnaround;

    std::ofstream outFile("../data/output.json");
    outFile << output_json.dump(4);
    std::cout << "FCFS scheduling complete. Results written to output.json\n";
}
