// C++ 线程的使用
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <condition_variable>   // 条件变量
#include <queue>

using namespace std;

// 模拟死锁
mutex mtx1;
mutex mtx2;

void threadA_func()
{
    // 获取锁1
    lock_guard<mutex> lock1(mtx1);
    this_thread::sleep_for(chrono::microseconds(100));

    // 试图获取锁2 (被线程 B 阻止, 会处于阻塞状态)
    lock_guard<mutex> lock2(mtx2);
    cout << "ThreadA is Done...\n";
}

void threadB_func()
{
    // 获取锁2
    lock_guard<mutex> lock2(mtx2);
    this_thread::sleep_for(chrono::microseconds(100));

    // 试图获取锁1 (被线程 A 阻止, 会处于阻塞状态)
    lock_guard<mutex> lock1(mtx1);
    cout << "ThreadB is Done...\n";
}

// 如何避免死锁？

// 方法1 (传统方法): 固定顺序上锁
void threadA_safe_tradition()
{
    // 获取锁1 -> 锁2
    lock_guard<mutex> lock1(mtx1);
    cout << "Thread A 获取锁1...\n";
    this_thread::sleep_for(chrono::microseconds(100));

    lock_guard<mutex> lock2(mtx2);
    cout << "Thread A 获取锁2...\n";
    cout << "Thread A is 安全执行...\n";
}

void threadB_safe_tradition()
{
    // 获取锁2
    lock_guard<mutex> lock1(mtx1);
    cout << "Thread B 获取锁1...\n";
    this_thread::sleep_for(chrono::microseconds(100));

    // 试图获取锁1 (被线程 A 阻止, 会处于阻塞状态)
    lock_guard<mutex> lock2(mtx2);
    cout << "Thread B 获取锁2...\n";
    cout << "ThreadB is Done...\n";
}

// 方法2 (现代做法): scoped_lock (C++17)
// 机制：请求同时对两个 mutex 对象上锁，并自动按顺序获取锁
void threadA_safe_modern()
{
    // 同时请求对 mtx1 和 mtx2 上锁
    // 请求失败，则释放已有锁并重试，直到同时上锁成功
    scoped_lock(mtx1, mtx2);
    cout << "Thread A 安全执行...\n";
}

void threadB_safe_modern()
{
    scoped_lock(mtx1, mtx2);
    cout << "Thread B 安全执行...\n";
}

int main()
{
    thread tA(threadA_safe_tradition);
    thread tB(threadB_safe_tradition);

    // 直接运行会导致两个线程同时阻塞，程序卡死，无输出
    tA.join();
    tB.join();

    std::cout << "All threads finished!!!\n";
}
