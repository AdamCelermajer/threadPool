# ThreadPool Implementation in C++

This project demonstrates a simple implementation of a thread pool in C++. A thread pool is a collection of threads that are used to execute tasks asynchronously. This implementation supports adding tasks with return values, non-blocking execution, and task synchronization.

## Features

- **Task Queue**: Tasks are added to a queue and executed by available worker threads.
- **Multithreading**: A fixed number of threads are used to process tasks concurrently.
- **Task Synchronization**: Task results are handled using `std::future` and `std::packaged_task`.
- **Graceful Shutdown**: The pool ensures that all threads are properly joined and terminated.

## Structure

- `ThreadPool` class:
  - **Constructor**: Initializes the thread pool with a specified number of threads.
  - **Destructor**: Ensures that all threads are joined before the pool is destroyed.
  - **addTask()**: Adds a task to the pool and returns a `std::future` for getting the result.

## How to Use

### 1. Creating a ThreadPool

To create a thread pool with a specified number of threads:

```cpp
ThreadPool pool(4);  // Creates a pool with 4 threads
