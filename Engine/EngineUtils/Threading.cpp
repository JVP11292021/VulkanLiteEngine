#include "Threading.hpp"

VLE_UTILS_NS_B

Thread::Thread()
    : started_(false),
      stopped_(false),
      paused_(false),
      pausing_(false),
      finished_(false),
      setup_(false),
      setup_valid_(false)
{
    this->registerCallback(STARTED_CALLBACK);
    this->registerCallback(FINISHED_CALLBACK);
}

ThreadStatus Thread::start() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if (started_ && !finished_) {
        return ThreadStatus::INVALID_START;
    }
    this->wait();
    this->timer_.restart();
    this->thread_ = std::thread(&Thread::runFunc, this);
    this->started_ = true;
    this->stopped_ = false;
    this->paused_ = false;
    this->pausing_ = false;
    this->finished_ = false;
    this->setup_ = false;
    this->setup_valid_ = false;
    return ThreadStatus::SUCCESS;
}

void Thread::stop() {
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        this->stopped_ = true;
    }
    this->resume();
}

void Thread::pause() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    this->paused_ = true;
}

void Thread::resume() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if (this->paused_) {
        this->paused_ = false;
        this->pause_condition_.notify_all();
    }
}

void Thread::wait() {
    if (this->thread_.joinable()) {
        this->thread_.join();
    }
}

bool Thread::isStarted() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->started_;
}

bool Thread::isStopped() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->stopped_;
}

bool Thread::isPaused() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->paused_;
}

bool Thread::isRunning() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->started_ && !this->pausing_ && !this->finished_;
}

bool Thread::isFinished() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->finished_;
}

ThreadStatus Thread::addCallback(const int id, std::function<void()> func) {
    if(this->callbacks_.count(id) > 0) return ThreadStatus::CALLBACK_NOT_REGISTERED;
    this->callbacks_.at(id).push_back(std::move(func));
    return ThreadStatus::SUCCESS;
}

void Thread::registerCallback(const int id) {
    this->callbacks_.emplace(id, std::list<std::function<void()>>());
}

ThreadStatus Thread::callback(const int id) const {
    if(this->callbacks_.count(id) > 0) return ThreadStatus::CALLBACK_NOT_REGISTERED;
    for (const auto& callback : this->callbacks_.at(id)) {
        callback();
    }
    return ThreadStatus::SUCCESS;
}

std::thread::id Thread::getThreadId() const {
    return std::this_thread::get_id();
}

ThreadStatus Thread::signalValidSetup() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if(this->setup_) return ThreadStatus::INVALID_SETUP;
    this->setup_ = true;
    this->setup_valid_ = true;
    this->setup_condition_.notify_all();
    return ThreadStatus::SUCCESS;
}

ThreadStatus Thread::signalInvalidSetup() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if(this->setup_) return ThreadStatus::INVALID_SETUP;
    this->setup_ = true;
    this->setup_valid_ = false;
    this->setup_condition_.notify_all();
    return ThreadStatus::SUCCESS;
}

const class Timer& Thread::getTimer() const { return timer_; }

void Thread::blockIfPaused() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this->paused_) {
        this->pausing_ = true;
        this->timer_.pause();
        this->pause_condition_.wait(lock);
        this->pausing_ = false;
        this->timer_.resume();
    }
}

bool Thread::checkValidSetup() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if (!this->setup_) {
        this->setup_condition_.wait(lock);
    }
    return this->setup_valid_;
}

void Thread::runFunc() {
    this->callback(STARTED_CALLBACK);
    this->run();
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        this->finished_ = true;
        this->timer_.pause();
    }
    this->callback(FINISHED_CALLBACK);
}

ThreadPool::ThreadPool(const int num_threads)
    : stopped_(false), num_active_workers_(0)
{
    const int num_effective_threads = GetEffectiveNumThreads(num_threads);
    for (int index = 0; index < num_effective_threads; ++index) {
        std::function<void(void)> worker =
            std::bind(&ThreadPool::workerFunc, this, index);
        this->workers_.emplace_back(worker);
    }
}

ThreadPool::~ThreadPool() { this->stop(); }

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(this->mutex_);

        if (this->stopped_) {
            return;
        }

        this->stopped_ = true;

        std::queue<std::function<void()>> empty_tasks;
        std::swap(this->tasks_, empty_tasks);
    }

    this->task_condition_.notify_all();

    for (auto& worker : this->workers_) {
        worker.join();
    }

    this->finished_condition_.notify_all();
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    if (!this->tasks_.empty() || this->num_active_workers_ > 0) {
        this->finished_condition_.wait(
            lock, [this]() { return this->tasks_.empty() && this->num_active_workers_ == 0; });
    }
}

void ThreadPool::workerFunc(const int index) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        this->thread_id_to_index_.emplace(this->getThreadId(), index);
    }

    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->mutex_);
            this->task_condition_.wait(lock,
                               [this] { return this->stopped_ || !this->tasks_.empty(); });
            if (this->stopped_ && this->tasks_.empty()) {
                return;
            }
            task = std::move(this->tasks_.front());
            this->tasks_.pop();
            this->num_active_workers_ += 1;
        }

        task();

        {
            std::unique_lock<std::mutex> lock(this->mutex_);
            this->num_active_workers_ -= 1;
        }

            this->finished_condition_.notify_all();
    }
}

std::thread::id ThreadPool::getThreadId() const {
    return std::this_thread::get_id();
}

int ThreadPool::getThreadIndex() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    return this->thread_id_to_index_.at(this->getThreadId());
}

int GetEffectiveNumThreads(const int num_threads) {
    int num_effective_threads = num_threads;
    if (num_threads <= 0) {
        num_effective_threads = std::thread::hardware_concurrency();
    }

    if (num_effective_threads <= 0) {
        num_effective_threads = 1;
    }

    return num_effective_threads;
}

VLE_UTILS_NS_E
