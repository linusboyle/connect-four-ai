#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

class Timer {
    std::clock_t start_time;
public:
    Timer() : start_time(std::clock()) {}
    double elapsed() const {
        return double(std::clock() - start_time) / CLOCKS_PER_SEC;
    }
};
#endif
