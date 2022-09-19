#pragma once

#include <functional>
#include <iostream>

class IScheduler {
public:
    virtual void Schedule(std::function<void(void)> f) = 0;

    virtual void Wait() = 0;
};

class InlineScheduler : public IScheduler {
public:
    void Schedule(std::function<void(void)> f) override {
        f();
    }

    /// Nothing to wait
    void Wait() override {}
};

#include <thread>
#include <list>

class SimpleScheduler : public IScheduler {
    std::list<std::thread> threads;

public:
    void Schedule(std::function<void(void)> f) override {
        threads.emplace_back(f);
    }

    /// Nothing to wait
    void Wait() override {
        for (auto &&t: threads) {
            t.join();
        }
    }
};

#include <condition_variable>
#include <optional>

class ThreadPoolSchedulerStack : public IScheduler {
    std::vector<std::thread> threads;

    class TaskPool {
    private:
        std::vector<std::function<void(void)>> stack;
        std::mutex mx;
        std::condition_variable cv;
        bool done = false;

    public:
        void put(std::function<void(void)> f) {
            std::unique_lock lock(mx);
            stack.push_back(std::move(f));
            cv.notify_one();
        }

        std::optional<std::function<void(void)>> get() {
            std::unique_lock lock(mx);
            while (true) {
                if (done && stack.empty()) {
                    return std::nullopt;
                }
                if (!stack.empty()) {
                    auto job = std::move(stack.back());
                    stack.pop_back();
                    return job;
                }
                cv.wait(lock);
            }
        }

        void stop() {
            std::unique_lock lock(mx);
            done = true;
        }
    } queue;

    void worker() {
        while (true) {
            auto f = queue.get();
            if (!f.has_value()) {
                return;
            }
            f.value()();
        }
    }

public:
    ThreadPoolSchedulerStack(size_t max_threads) {
        for (size_t i = 0; i != max_threads; ++i) {
            threads.emplace_back([this]() {
                worker();
            });
        }
    }

    void Schedule(std::function<void(void)> f) override {
        queue.put(std::move(f));
    }

    /// Nothing to wait
    void Wait() override {
        queue.stop();
        for (auto &&t: threads) {
            t.join();
        }
    }
};


#include <queue>

class ThreadPoolSchedulerQueue : public IScheduler {
    std::vector<std::thread> threads;

    class TaskPool {
    private:
        std::queue<std::function<void(void)>> queue;
        std::mutex mx;
        std::condition_variable cv;
        bool done = false;

    public:
        void put(std::function<void(void)> f) {
            std::unique_lock lock(mx);
            queue.push(std::move(f));
            cv.notify_one();
        }

        std::optional<std::function<void(void)>> get() {
            std::unique_lock lock(mx);
            while (true) {
                if (done && queue.empty()) {
                    return std::nullopt;
                }
                if (!queue.empty()) {
                    auto job = std::move(queue.front());
                    queue.pop();
                    return job;
                }
                cv.wait(lock);
            }
        }

        void stop() {
            std::unique_lock lock(mx);
            done = true;
        }
    } queue;

    void worker() {
        while (true) {
            auto f = queue.get();
            if (!f.has_value()) {
                return;
            }
            f.value()();
        }
    }

public:
    ThreadPoolSchedulerQueue(size_t max_threads) {
        for (size_t i = 0; i != max_threads; ++i) {
            threads.emplace_back([this]() {
                worker();
            });
        }
    }

    void Schedule(std::function<void(void)> f) override {
        queue.put(std::move(f));
    }

    /// Nothing to wait
    void Wait() override {
        queue.stop();
        for (auto &&t: threads) {
            t.join();
        }
    }
};
