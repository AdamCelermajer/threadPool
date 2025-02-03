
#include <vector>             // For std::vector
#include <queue>              // For std::queue
#include <thread>             // For std::thread
#include <mutex>              // For std::mutex
#include <functional>         // For std::function
#include <condition_variable> // For std::condition_variable
#include <iostream>           // For std::cout, std::endl
#include <future>             // For std::future

using namespace std;


class ThreadPool{

private:
int numThreads;
vector<thread> myThreads;
queue<function<void()>> myQueue;
mutex myMutex;
condition_variable myConditionVariable;
bool stop = false;

void workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(myMutex);
            myConditionVariable.wait(lock, [this] { return stop || !myQueue.empty(); });

            if (stop && myQueue.empty()) return;  

            task = std::move(myQueue.front());
            myQueue.pop();
        }
        task(); 
    }
}

public:
ThreadPool(int numThreads): numThreads(numThreads){
    for (int i=0;i<numThreads;i++){
        myThreads.emplace_back([this] {workerThread();});
    }
    }

~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(myMutex);
        stop = true;
    }
    myConditionVariable.notify_all(); // Wake up all waiting threads

    for (std::thread &t : myThreads) {
        if (t.joinable()) {
            t.join(); // Wait for thread to finish
        }
    }
}

template <typename F, typename... Args>
auto addTask(const F& f, Args&&... args)-> std::future<decltype(f(args...))>  
{
    using ReturnType = decltype(f(args...));  // Figure out the return type

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(f, std::forward<Args>(args)...)
    );
    std::future<ReturnType> future = task->get_future();

    {
        std::lock_guard<std::mutex> lock(myMutex);
        myQueue.push([task]() { 
        (*task)();});
    } 
    myConditionVariable.notify_one();
    return future;
}
};
int main() {
    // Create a thread pool with 4 threads
    ThreadPool pool(4);

    // Enqueue a task that returns an integer
    auto future1 = pool.addTask([]() {
        std::cout << "Task 1 is running." << std::endl;
        return 42;
    });

    // Enqueue a task that takes parameters and returns their sum
    auto future2 = pool.addTask([](int a, int b) {
        std::cout << "Task 2 is running." << std::endl;
        return a + b;
    }, 10, 20);

    // Enqueue a task that returns void
    auto future3 = pool.addTask([]() {
        std::cout << "Task 3 is running." << std::endl;
    });

    // Sleep for 1 second to allow tasks to run
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Retrieve and display the results
    std::cout << "Result from Task 1: " << future1.get() << std::endl;
    std::cout << "Result from Task 2: " << future2.get() << std::endl;
    future3.get(); // Ensure Task 3 completes

    return 0;
}
