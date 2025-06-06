// backend/scheduler.h

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <algorithm>

struct Process {
    std::string pid;
    int arrival;
    int burst;
    int priority;
    int remaining;
    int start_time = -1;
    int finish_time = -1;
};

struct GanttSegment {
    std::string pid;
    int start;
    int end;
};

struct Result {
    std::string pid;
    int waiting;
    int turnaround;
};

struct Output {
    std::vector<GanttSegment> gantt;
    std::vector<Result> results;
    double avg_waiting;
    double avg_turnaround;
};

#endif // SCHEDULER_H
