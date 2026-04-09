#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// A simple Thread Pool implementation to manage multiple concurrent execution threads
class ThreadPool {
private:
  // Vector to store the worker threads
  std::vector<std::thread> workers;
  
  // Queue to store tasks (functions) that need to be executed
  std::queue<std::function<void()>> tasks;

  // Mutex to protect access to the task queue
  std::mutex queue_mutex;
  
  // Condition variable to notify worker threads when new tasks are available
  std::condition_variable condition;
  
  // Flag to indicate whether the thread pool should stop accepting new tasks and shut down
  bool stop = false;

public:
  // Constructor: Initializes the thread pool with the specified number of threads
  ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
      // Each worker thread continuously looks for new tasks in the queue
      workers.emplace_back([this]() {
        while (true) {
          std::function<void()> task;

          {
            // Acquire lock before checking the queue or waiting
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // Wait until a task is available or the pool is stopped
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            // If the pool is stopped and there are no more tasks, exit the thread
            if (stop && tasks.empty())
              return;

            // Retrieve the next task from the front of the queue
            task = std::move(tasks.front());
            tasks.pop();
          }

          // Execute the retrieved task outside the lock
          task();
        }
      });
    }
  }

  // Enqueue: Adds a new task to the queue for the worker threads to execute
  void enqueue(std::function<void()> task) {
    {
      // Lock the queue to safely add the new task
      std::lock_guard<std::mutex> lock(queue_mutex);
      tasks.push(task);
    }
    // Notify one of the waiting threads that a new task is available
    condition.notify_one();
  }

  // Destructor: Cleans up the thread pool by joining all worker threads
  ~ThreadPool() {
    {
      // Lock the queue to safely set the stop flag
      std::lock_guard<std::mutex> lock(queue_mutex);
      stop = true;
    }
    
    // Wake up all threads so they can finish any remaining tasks and then exit
    condition.notify_all();

    // Wait for all worker threads to complete their execution
    for (std::thread &worker : workers)
      worker.join();
  }
};

int main() {
  // Create a thread pool with 4 worker threads
  ThreadPool pool(4);

  // Enqueue 10 tasks to be executed by the thread pool
  for (int i = 0; i < 10; i++) {
    // Each task consists of printing its execution number
    pool.enqueue([i]() { std::cout << "Task " << i << " executed\n"; });
  }

  // Thread pool destructor is automatically called here, waiting for tasks to finish
  return 0;
}