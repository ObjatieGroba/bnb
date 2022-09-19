#pragma once

#include <iostream>
#include <atomic>
#include <array>

namespace Metrics {
enum Type {
    kSolve = 0,
    kBound = 1,
    kSplit = 2,

    kNum = 3
};

struct Metric {
    std::array<std::atomic_uint, kNum> data_;

    void reset() {
        for (auto && elem : data_) {
            elem = 0;
        }
    }

    void add(Type t, unsigned add = 1) {
        data_[t].fetch_add(add);
    }

    void print() {
        std::cout << "Solve calls: " << data_[kSolve] << std::endl;
        std::cout << "Bound calls: " << data_[kBound] << std::endl;
        std::cout << "Split calls: " << data_[kSplit] << std::endl;
    }
} metric;
}
