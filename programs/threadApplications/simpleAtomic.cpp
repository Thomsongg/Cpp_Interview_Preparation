// C++ 线程的使用
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <atomic>       // 原子操作
#include <chrono>       // 计时

using namespace std;

// 原子操作
// 两种方式，实现简单的操作：如变量自加

// 方式1: 普通 int + 互斥锁 -> 需要操作系统控制，开销大
int count_mutex = 0;
mutex mtx;

template<const int ITERATIONS>
void work_mutex()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        lock_guard<mutex> lock(mtx);
        count_mutex++;
    }
}

// 方式2: std::atomic -> 无锁操作，开销小(CPU 实现全过程)，线程安全
atomic<int> count_atomic(0);

template<const int ITERATIONS>
void work_atomic()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // 原子变量自加
        // 也可使用原子方法：等价于 count_atomic.fetch_add(1)
        count_atomic++;
    }
}


int main()
{
    const int iterations = 100000;
    const int num_threads = 10;

    vector<thread> threads;
    threads.reserve(num_threads);

    // 方法1 开始计时
    cout << "开始测试方法1：使用互斥锁 (mutex)...\n";
    auto start_mtx = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(work_mutex<iterations>);
    }
    
    for (auto& thread : threads)
    {
        thread.join();
    }

    // 方法1 计时结束
    auto end_mtx = chrono::high_resolution_clock::now();
    auto duration_mtx = chrono::duration_cast<chrono::milliseconds>(end_mtx - start_mtx);
    threads.clear();

    // 方法2
    cout << "开始测试方法2：原子操作 (atomic)...\n";
    auto start_atm = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(work_atomic<iterations>);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    auto end_atm = chrono::high_resolution_clock::now();
    auto duration_atm = chrono::duration_cast<chrono::milliseconds>(end_atm - start_atm);

    // 输出结果，进行比较
    cout << "Mutex 累加结果: " << count_mutex << " | 耗时: " << duration_mtx.count() << "ms\n";
    cout << "Atomic 累加结果: " << count_atomic << " | 耗时: " << duration_atm.count() << "ms\n";
    cout << "性能倍数: Atomic 比 Mutex 快了约 " << (double)duration_mtx.count() / (double)duration_atm.count() << " 倍\n";

    std::cout << "All threads finished!!!\n";
}
