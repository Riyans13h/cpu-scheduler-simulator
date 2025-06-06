#include <iostream>
#include <string>
#include <vector>
#include "scheduler.h"
#include <map>

std::vector<Result> fcfs(std::vector<Process>);
std::vector<Result> sjf(std::vector<Process>);
std::vector<Result> srtf(std::vector<Process>);
std::vector<Result> rr(std::vector<Process>);
std::vector<Result> priority_scheduling(std::vector<Process>, bool);
std::vector<Result> hrrn(std::vector<Process>);
std::vector<Result> ljf(std::vector<Process>);
std::vector<Result> lrtf(std::vector<Process>);
std::vector<Result> mqs(std::vector<Process>);
std::vector<Result> mlfq(std::vector<Process>);

void calculateMetrics(const std::vector<Process>& processes, const std::vector<Result>& gantt, float& avg_wait, float& avg_tat) {
    std::map<int, int> completion_time;
    for (const auto& r : gantt) {
        completion_time[r.pid] = r.end_time;
    }

    float total_wait = 0, total_tat = 0;
    for (const auto& p : processes) {
        int tat = completion_time[p.pid] - p.arrival_time;
        int wt = tat - p.burst_time;
        total_wait += wt;
        total_tat += tat;
    }

    avg_wait = total_wait / processes.size();
    avg_tat = total_tat / processes.size();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./main <algorithm>" << std::endl;
        return 1;
    }

    std::string algorithm = argv[1];
    std::vector<Process> processes = parseInputJSON("../data/input.json");
    std::vector<Result> gantt;
    float avg_wait = 0, avg_tat = 0;

    if (algorithm == "fcfs") {
        gantt = fcfs(processes);
    } else if (algorithm == "sjf") {
        gantt = sjf(processes);
    } else if (algorithm == "srtf") {
        gantt = srtf(processes);
    } else if (algorithm == "rr") {
        gantt = rr(processes);
    } else if (algorithm == "priority") {
        gantt = priority_scheduling(processes, true); // true = preemptive
    } else if (algorithm == "hrrn") {
        gantt = hrrn(processes);
    } else if (algorithm == "ljf") {
        gantt = ljf(processes);
    } else if (algorithm == "lrtf") {
        gantt = lrtf(processes);
    } else if (algorithm == "mqs") {
        gantt = mqs(processes);
    } else if (algorithm == "mlfq") {
        gantt = mlfq(processes);
    } else {
        std::cerr << "Unknown algorithm: " << algorithm << std::endl;
        return 1;
    }

    calculateMetrics(processes, gantt, avg_wait, avg_tat);
    writeOutputJSON("../data/output.json", gantt, avg_wait, avg_tat);

    std::cout << "Scheduling complete. Output written to output.json" << std::endl;
    return 0;
}
