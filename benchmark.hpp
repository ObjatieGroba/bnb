#pragma once

#include "IScheduler.h"
#include "IBnB.h"

#include "testfuncs.hpp"
#include "expression/expr.hpp"
#include "BnBSimpleSolver.h"
#include <chrono>
#include "metrics.h"

using namespace snowgoose::expression;
using namespace snowgoose::derivative;
using namespace OPTITEST;

class Ackley1Task : public IBnBTask<Interval<double>, double, double> {
public:
    double predict(Interval<double> interval) {
        Metrics::metric.add(Metrics::kBound);
        return ::calcInterval(Ackley1<Interval<double>>(1), {interval}).lb();
    }

    std::pair<double, double> solve(Interval<double> interval) {
        Metrics::metric.add(Metrics::kSolve);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto mid = interval.mid();
        return {::calcFunc(Ackley1<double>(1), {mid}), mid};
    }

    std::vector<Interval<double>> split(Interval<double> interval) {
        Metrics::metric.add(Metrics::kSplit);
        if (interval.size() < 0.000001) {
            return {};
        }
        auto mid = interval.mid();
        return {
            {interval.lb(), mid},
            {mid, interval.rb()},
        };
    }

    double answer() const {
        return 0;
    }

    Interval<double> start() const {
        return {-64, 32};
    }
};

class GriewankTask : public IBnBTask<Interval<double>, double, double> {
public:
    double predict(Interval<double> interval) {
        Metrics::metric.add(Metrics::kBound);
        return ::calcInterval(Griewank<Interval<double>>(1), {interval}).lb();
    }

    std::pair<double, double> solve(Interval<double> interval) {
        Metrics::metric.add(Metrics::kSolve);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto mid = interval.mid();
        return {::calcFunc(Griewank<double>(1), {mid}), mid};
    }

    std::vector<Interval<double>> split(Interval<double> interval) {
        Metrics::metric.add(Metrics::kSplit);
        if (interval.size() < 0.00001) {
            return {};
        }
        auto mid = interval.mid();
        return {
            {interval.lb(), mid},
            {mid, interval.rb()},
        };
    }

    double answer() const {
        return 0;
    }

    Interval<double> start() const {
        return {-64, 32};
    }
};

