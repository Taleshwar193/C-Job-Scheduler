#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop = false;

public:
  ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
      workers.emplace_back([this]() {
        while (true) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
              return;

            task = std::move(tasks.front());
            tasks.pop();
          }

          task();
        }
      });
    }
  }

  void enqueue(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      tasks.push(task);
    }
    condition.notify_one();
  }

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();

    for (std::thread &worker : workers)
      worker.join();
  }
};

int main() {
  ThreadPool pool(4);

  for (int i = 0; i < 10; i++) {
    pool.enqueue([i]() { std::cout << "Task " << i << " executed\n"; });
  }

  return 0;
}