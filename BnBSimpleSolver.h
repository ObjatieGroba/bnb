#include <IScheduler.h>
#include <IBnB.h>
#include <limits>
#include <mutex>


template <class Point, class Value>
class SharedMinValue {
    Point point_;
    std::atomic<Value> min_;
    std::mutex mx;
public:
    SharedMinValue(Point point, Value value): point_(std::move(point)), min_(std::move(value)) {}

    Value get_min_nonblock() {
        return min_.load();
    }

    void Update(Point point, Value value) {
        if (value > get_min_nonblock()) {
            return;
        }
        std::unique_lock lock(mx);
        if (value > min_) {
            return;
        }
        min_ = value;
        point_ = point;
    }

    std::pair<Point, Value> Result() {
        std::unique_lock lock(mx);
        return {point_, min_};
    }
};


template <class Box, class Point, class Value>
class BnBSimpleSolver : public IBnBSolver<IBnBTask<Box, Point, Value>, Point, Value> {
    IScheduler &scheduler;
    IBnBTask<Box, Point, Value> &task;

public:
    BnBSimpleSolver(IScheduler &scheduler, IBnBTask<Box, Point, Value> &task) : scheduler(scheduler), task(task) {}

    std::pair<Point, Value> solve() override {
        auto start_box = task.start();
        auto [min_value, min_point] = task.solve(start_box);
        SharedMinValue min(std::move(min_point), std::move(min_value));

        std::function<void(Box)> process = [&min, &process, this](Box box) {
            auto predicted_min = task.predict(box);
            if (predicted_min > min.get_min_nonblock()) {
                return;
            }
            auto [val, point] = task.solve(box);
            min.Update(point, val);
            auto split = task.split(box);
            for (auto &&next_box : split) {
                scheduler.Schedule([&, next_box]() {
                    process(next_box);
                });
            }
        };

        scheduler.Schedule([&process, start_box](){
            process(start_box);
        });
        scheduler.Wait();
        return min.Result();
    }
};
