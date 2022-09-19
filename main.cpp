#include "benchmark.hpp"
#include <iostream>
#include <chrono>
#include "metrics.h"

void process(IScheduler &sched, IBnBTask<Interval<double>, double, double> &task) {
    Metrics::metric.reset();
    const auto start = std::chrono::system_clock::now();
    BnBSimpleSolver<Interval<double>, double, double> s(sched, task);
    auto res = s.solve();
    const auto end = std::chrono::system_clock::now();
    unsigned long int mseconds = (std::chrono::duration_cast<std::chrono::microseconds> (end - start)).count();
    std::cout << "Time: " << mseconds << " microsecond\n";
    std::cout << "Result: " << res.first << ' ' << task.answer() << std::endl;
    Metrics::metric.print();
    std::cout << std::endl;
}

int main() {
//    Ackley1Task task;
    GriewankTask task;
    {
        InlineScheduler sched;
        process(sched, task);
    }

    {
        SimpleScheduler sched;
        process(sched, task);
    }

    {
        ThreadPoolSchedulerQueue sched(2);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerQueue sched(4);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerQueue sched(8);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerQueue sched(16);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerStack sched(2);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerStack sched(4);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerStack sched(8);
        process(sched, task);
    }

    {
        ThreadPoolSchedulerStack sched(16);
        process(sched, task);
    }
}
