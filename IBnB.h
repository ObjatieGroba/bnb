#pragma once

#include <utility>
#include <optional>
#include <array>
#include <vector>

template <class Box_, class Point_, class Value_>
class IBnBTask {
public:
    using Box = Box_;
    using Value = Value_;
    using Point = Point_;

    /// Выдает нижнюю границу ответа
    virtual Value predict(Box interval) = 0;

    /// Выдает ответ
    virtual std::pair<Value, Point> solve(Box interval) = 0;

    /// Разбивает интервал если возможно
    virtual std::vector<Box> split(Box interval) = 0;

    /// Ответ для тестов
    virtual Point answer() const = 0;

    /// Начальный интервал
    virtual Box start() const = 0;
};

template <class BnBTask, class Point, class Value>
class IBnBSolver {
    virtual std::pair<Point, Value> solve() = 0;
};
