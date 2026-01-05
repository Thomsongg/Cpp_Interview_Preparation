// C++ 线程的使用
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// 题目1: 线程安全的计数器
// 启动 10 个线程，执行 10w 次自增操作，用三种方法
// (1) 无保护，直接操作
// (2) 使用线程锁
// (3) 使用原子操作 atomic<int>

int counter = 0;

// 方法1：无保护
template<const int ITERATIONS>
void count()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter++;
    }
}

// 方法2：使用线程锁 mutex
mutex mtx;

template<const int ITERATIONS>
void count_safe()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // 上锁后，再进行操作 -> 细粒度锁
        lock_guard<mutex> lock(mtx);
        counter++;
    }
}

// 【串行化】如果把上锁操作放在循环外，则有如下实现
// 每个线程的执行变为串行化，在本例中效率较高（甚至强于 Atomic 方法），但失去了多线程并行性
template<const int ITERATIONS>
void count_safe_serial()
{
    // 对整个循环上锁，循环进行时阻塞其他线程 -> 粗粒度锁
    lock_guard<mutex> lock(mtx);
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter++;
    }
}

// 方法3：使用原子变量 atomic<int>
atomic<int> counter_atm(0);

template<const int ITERATIONS>
void count_atomic()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter_atm++;
    }
}

int main()
{
    // 创建 10 个线程
    const int iterations = 100000;
    const int num_threads = 10;

    vector<thread> threads;

    auto start_mtx = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(count_safe<iterations>);
    }

    // 启动线程
    for (auto& t : threads)
    {
        t.join();
    }

    auto end_mtx = chrono::high_resolution_clock::now();
    auto duration_mtx = chrono::duration_cast<chrono::microseconds>(end_mtx - start_mtx);
    threads.clear();

    auto start_atm = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(count_atomic<iterations>);
    }

    // 启动线程
    for (auto& t : threads)
    {
        t.join();
    }

    auto end_atm = chrono::high_resolution_clock::now();
    auto duration_atm = chrono::duration_cast<chrono::microseconds>(end_atm - start_atm);
    threads.clear();

    cout << "最终的结果为: " << counter_atm << "\n";
    cout << "Mutex 方法 耗时：" << duration_mtx.count() << "ms\n";
    cout << "Atomic 方法 耗时：" << duration_atm.count() << "ms\n";

    std::cout << "All threads finished!!!\n";
}
