#include "Timer.hpp"

VLE_UTILS_NS_B

Timer::Timer() : started_(false), paused_(false) {}

void Timer::start() {
    this->started_ = true;
    this->paused_ = false;
    this->start_time_ = std::chrono::high_resolution_clock::now();
}

void Timer::restart() {
    this->started_ = false;
    this->start();
}

void Timer::pause() {
    this->paused_ = true;
    this->pause_time_ = std::chrono::high_resolution_clock::now();
}

void Timer::resume() {
    this->paused_ = false;
    this->start_time_ += std::chrono::high_resolution_clock::now() - this->pause_time_;
}

void Timer::reset() {
    this->started_ = false;
    this->paused_ = false;
}

double Timer::elapsedMicroSeconds() const {
  if (!this->started_) {
    return 0.0;
  }
  if (this->paused_) {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            this->pause_time_ - this->start_time_).count();
  } else {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - this->start_time_).count();
  }
}

double Timer::elapsedSeconds() const { return this->elapsedMicroSeconds() / 1e6; }

double Timer::elapsedMinutes() const { return this->elapsedSeconds() / 60; }

double Timer::elapsedHours() const { return this->elapsedMinutes() / 60; }

VLE_UTILS_NS_E
