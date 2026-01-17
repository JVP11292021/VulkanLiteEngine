#ifndef VLE_UTILS_TIMER_H
#define VLE_UTILS_TIMER_H

#include "eutils.hpp"
#include <chrono>

VLE_UTILS_NS_B

class Timer {
public:
    Timer();

    void start();
    void restart();
    void pause();
    void resume();
    void reset();

    double elapsedMicroSeconds() const;
    double elapsedSeconds() const;
    double elapsedMinutes() const;
    double elapsedHours() const;

private:
    bool started_;
    bool paused_;
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point pause_time_;
};

VLE_UTILS_NS_E

#endif // VLE_UTILS_TIMER_H