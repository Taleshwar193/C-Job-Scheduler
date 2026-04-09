# C++ Multithreaded Task Scheduler (Thread Pool)

This project demonstrates a simple and effective implementation of a **Thread Pool** in C++ using the `<thread>`, `<mutex>`, and `<condition_variable>` standard library components. It provides a solid foundation for learning and demonstrating core multithreading concepts.

## Overview

A Thread Pool manages a group of pre-instantiated, idle threads which stand ready to be given work. When a task is added to the pool's queue, one of the idle threads is assigned to execute it. This is more efficient than continually creating and destroying threads for short-lived tasks.

## Key Components

- **`std::vector<std::thread> workers`**: Maintains a collection of active worker threads.
- **`std::queue<std::function<void()>> tasks`**: A queue of tasks waiting to be executed.
- **`std::mutex queue_mutex`**: Ensures thread-safe access to the task queue. Prevents race conditions when multiple threads attempt to add or remove tasks simultaneously.
- **`std::condition_variable condition`**: Used to block threads until a task becomes available in the queue, preventing high CPU usage from busy-waiting.

## How it Works

1. **Initialization**: The `ThreadPool` is created with a specified number of worker threads. These threads immediately start running an infinite loop, constantly waiting for new tasks.
2. **Enqueuing Tasks**: When the `enqueue` function is called, a new task (encapsulated in a `std::function<void()>`) is pushed onto the queue. The mutex ensures this is done safely. The condition variable then wakes up one of the idle threads.
3. **Task Execution**: A thread wakes up, acquires the lock, retrieves a task off the queue, releases the lock, and executes the task.
4. **Shutdown**: When the `ThreadPool` goes out of scope, its destructor is called. It sets a `stop` flag, wakes up all threads using `notify_all()`, and waits for them to join (finish their current work and exit their loops).

## Getting Started

### Prerequisites

- A C++11 (or later) compatible compiler (e.g., GCC, Clang, MSVC).

### Compilation and Execution

Use `g++` (or another modern C++ compiler) to compile the code. You will need to link the pthread library if you are compiling on a POSIX-compliant system (like Linux). On macOS, threads are supported and linked automatically.

```bash
g++ -std=c++11 Code.cpp -o task_scheduler
./task_scheduler
```

### Expected Output
The order of the task execution will vary during different runs, reflecting the asynchronous nature of multithreading.

```text
Task 0 executed
Task 1 executed
Task 2 executed
Task 3 executed
Task 5 executed
Task 4 executed
Task 6 executed
Task 7 executed
Task 8 executed
Task 9 executed
```
