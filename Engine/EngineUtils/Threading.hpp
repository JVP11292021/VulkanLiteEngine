#ifndef VLE_UTILS_THREADING_LIB_H
#define VLE_UTILS_THREADING_LIB_H

#include "Timer.hpp"

#include <climits>
#include <functional>
#include <future>
#include <list>
#include <queue>
#include <thread>
#include <type_traits>
#include <unordered_map>

VLE_UTILS_NS_B

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#ifdef __clang__
#pragma clang diagnostic pop  // -Wkeyword-macro
#endif

enum class ThreadStatus : std::int8_t {
    SUCCESS = 0x01,
    FATAL_FAILURE = 0x00,
    CALLBACK_NOT_REGISTERED = -0x01,
    INVALID_SETUP = -0x02,
    INVALID_START = -0x03
};

// Helper class to create single threads with simple controls and timing, e.g.:
//
//      class MyThread : public Thread {
//        enum {
//          PROCESSED_CALLBACK,
//        };
//
//        MyThread() { RegisterCallback(PROCESSED_CALLBACK); }
//        void Run() {
//          // Some setup routine... note that this optional.
//          if (setup_valid) {
//            SignalValidSetup();
//          } else {
//            SignalInvalidSetup();
//          }
//
//          // Some pre-processing...
//          for (const auto& item : items) {
//            BlockIfPaused();
//            if (IsStopped()) {
//              // Tear down...
//              break;
//            }
//            // Process item...
//            Callback(PROCESSED_CALLBACK);
//          }
//        }
//      };
//
//      MyThread thread;
//      thread.AddCallback(MyThread::PROCESSED_CALLBACK, []() {
//        LOG(INFO) << "Processed item"; })
//      thread.AddCallback(MyThread::STARTED_CALLBACK, []() {
//        LOG(INFO) << "Start"; })
//      thread.AddCallback(MyThread::FINISHED_CALLBACK, []() {
//        LOG(INFO) << "Finished"; })
//      thread.Start();
//      // thread.CheckValidSetup();
//      // Pause, resume, stop, ...
//      thread.Wait();
//      thread.Timer().PrintElapsedSeconds();
//
class Thread {
public:
    enum {
        STARTED_CALLBACK = INT_MIN,
        FINISHED_CALLBACK,
    };

    Thread();
    virtual ~Thread() = default;

    // Control the state of the thread.
    virtual ThreadStatus start();
    virtual void stop();
    virtual void pause();
    virtual void resume();
    virtual void wait();

    // Check the state of the thread.
    bool isStarted();
    bool isStopped();
    bool isPaused();
    bool isRunning();
    bool isFinished();

    // To be called from inside the main run function. This blocks the main
    // caller, if the thread is paused, until the thread is resumed.
    void blockIfPaused();

    // To be called from outside. This blocks the caller until the thread is
    // setup, i.e. it signaled that its setup was valid or not. If it never gives
    // this signal, this call will block the caller infinitely. Check whether
    // setup is valid. Note that the result is only meaningful if the thread gives
    // a setup signal.
    bool checkValidSetup();

    // Set callbacks that can be triggered within the main run function.
    ThreadStatus addCallback(int id, std::function<void()> func);

    // Get timing information of the thread, properly accounting for pause times.
    const Timer& getTimer() const;

protected:
    // This is the main run function to be implemented by the child class. If you
    // are looping over data and want to support the pause operation, call
    // `BlockIfPaused` at appropriate places in the loop. To support the stop
    // operation, check the `IsStopped` state and early return from this method.
    virtual void run() = 0;

    // Register a new callback. Note that only registered callbacks can be
    // set/reset and called from within the thread. Hence, this method should be
    // called from the derived thread constructor.
    void registerCallback(int id);

    // Call back to the function with the specified name, if it exists.
    ThreadStatus callback(int id) const;

    // Get the unique identifier of the current thread.
    std::thread::id getThreadId() const;

    // Signal that the thread is setup. Only call this function once.
    ThreadStatus signalValidSetup();
    ThreadStatus signalInvalidSetup();

private:
    // Wrapper around the main run function to set the finished flag.
    void runFunc();

private:
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable pause_condition_;
    std::condition_variable setup_condition_;

    Timer timer_;

    bool started_;
    bool stopped_;
    bool paused_;
    bool pausing_;
    bool finished_;
    bool setup_;
    bool setup_valid_;

    std::unordered_map<int, std::list<std::function<void()>>> callbacks_;
};

// A thread pool class to submit generic tasks (functors) to a pool of workers:
//
//    ThreadPool thread_pool;
//    thread_pool.AddTask([]() { /* Do some work */ });
//    auto future = thread_pool.AddTask([]() { /* Do some work */ return 1; });
//    const auto result = future.get();
//    for (int i = 0; i < 10; ++i) {
//      thread_pool.AddTask([](const int i) { /* Do some work */ });
//    }
//    thread_pool.Wait();
//
class ThreadPool {
public:
    static const int kMaxNumThreads = -1;

    template <class func_t, class... args_t>
#ifdef __cpp_lib_is_invocable
    using result_of_t = std::invoke_result_t<func_t, args_t...>;
#else
    using result_of_t = typename std::result_of<func_t(args_t...)>::type;
#endif

    explicit ThreadPool(int num_threads = kMaxNumThreads);
    ~ThreadPool();

    inline size_t NumThreads() const;

    // Add new task to the thread pool.
    template <class func_t, class... args_t>
    auto addTask(func_t&& f, args_t&&... args)
      -> std::future<result_of_t<func_t, args_t...>>;

    // Stop the execution of all workers.
    void stop();

    // Wait until tasks are finished.
    void wait();

    // Get the unique identifier of the current thread.
    std::thread::id getThreadId() const;

    // Get the index of the current thread. In a thread pool of size N,
    // the thread index defines the 0-based index of the thread in the pool.
    // In other words, there are the thread indices 0, ..., N-1.
    int getThreadIndex();

private:
    void workerFunc(int index);

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable task_condition_;
    std::condition_variable finished_condition_;

    bool stopped_;
    int num_active_workers_;

    std::unordered_map<std::thread::id, int> thread_id_to_index_;
};

// A job queue class for the producer-consumer paradigm.
//
//    JobQueue<int> job_queue;
//
//    std::thread producer_thread([&job_queue]() {
//      for (int i = 0; i < 10; ++i) {
//        job_queue.Push(i);
//      }
//    });
//
//    std::thread consumer_thread([&job_queue]() {
//      for (int i = 0; i < 10; ++i) {
//        const auto job = job_queue.Pop();
//        if (job.IsValid()) { /* Do some work */ }
//        else { break; }
//      }
//    });
//
//    producer_thread.join();
//    consumer_thread.join();
//
template <typename T>
class JobQueue {
public:
    class Job {
    public:
        Job() : data_{}, valid_(false) {}
        explicit Job(T data) : data_(std::move(data)), valid_(true) {}

        // Check whether the data is valid.
        bool isValid() const { return valid_; }

        // Get reference to the data.
        T& data() { return data_; }
        const T& data() const { return data_; }

    private:
        T data_;
        bool valid_;
    };

    JobQueue();
    explicit JobQueue(size_t max_num_jobs);
    ~JobQueue();

    // The number of pushed and not popped jobs in the queue.
    size_t size();

    // Push a new job to the queue. Waits if the number of jobs is exceeded.
    bool push(T data);

    // Pop a job from the queue. Waits if there is no job in the queue.
    Job pop();

    // Wait for all jobs to be popped and then stop the queue.
    void wait();

    // Stop the queue and return from all push/pop calls with false.
    void stop();

    // Clear all pushed and not popped jobs from the queue.
    void clear();

private:
    size_t max_num_jobs_;
    bool stop_;
    std::queue<T> jobs_;
    std::mutex mutex_;
    std::condition_variable push_condition_;
    std::condition_variable pop_condition_;
    std::condition_variable empty_condition_;
};

// Return the number of logical CPU cores if num_threads <= 0,
// otherwise return the input value of num_threads.
int GetEffectiveNumThreads(int num_threads);

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

size_t ThreadPool::NumThreads() const { return workers_.size(); }

template <class func_t, class... args_t>
auto ThreadPool::addTask(func_t&& f, args_t&&... args)
    -> std::future<result_of_t<func_t, args_t...>> {
  typedef result_of_t<func_t, args_t...> return_t;

  auto task = std::make_shared<std::packaged_task<return_t()>>(
      std::bind(std::forward<func_t>(f), std::forward<args_t>(args)...));

  std::future<return_t> result = task->get_future();

  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (stopped_) {
      throw std::runtime_error("Cannot add task to stopped thread pool.");
    }
    tasks_.emplace([task]() { (*task)(); });
  }

  task_condition_.notify_one();

  return result;
}

template <typename T>
JobQueue<T>::JobQueue() : JobQueue(std::numeric_limits<size_t>::max()) {}

template <typename T>
JobQueue<T>::JobQueue(const size_t max_num_jobs)
    : max_num_jobs_(max_num_jobs), stop_(false) {}

template <typename T>
JobQueue<T>::~JobQueue() {
  this->stop();
}

template <typename T>
size_t JobQueue<T>::size() {
  std::unique_lock<std::mutex> lock(this->mutex_);
  return this->jobs_.size();
}

template <typename T>
bool JobQueue<T>::push(T data) {
    std::unique_lock<std::mutex> lock(this->mutex_);
    while (this->jobs_.size() >= this->max_num_jobs_ && !this->stop_) {
        this->pop_condition_.wait(lock);
    }
    if (this->stop_) {
        return false;
    } else {
        this->jobs_.push(std::move(data));
        this->push_condition_.notify_one();
        return true;
    }
}

template <typename T>
typename JobQueue<T>::Job JobQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    while (this->jobs_.empty() && !this->stop_) {
        this->push_condition_.wait(lock);
    }
    if (this->stop_) {
        return Job();
    } else {
        Job job(std::move(this->jobs_.front()));
        this->jobs_.pop();
        this->pop_condition_.notify_one();
        if (this->jobs_.empty()) {
            this->empty_condition_.notify_all();
        }
        return job;
    }
}

template <typename T>
void JobQueue<T>::wait() {
  std::unique_lock<std::mutex> lock(this->mutex_);
  while (!this->jobs_.empty()) {
      this->empty_condition_.wait(lock);
  }
}

template <typename T>
void JobQueue<T>::stop() {
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        this->stop_ = true;
    }
    this->push_condition_.notify_all();
    this->pop_condition_.notify_all();
}

template <typename T>
void JobQueue<T>::clear() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    std::queue<T> empty_jobs;
    std::swap(this->jobs_, empty_jobs);
}

VLE_UTILS_NS_E

#endif // VLE_UTILS_THREADING_LIB_H