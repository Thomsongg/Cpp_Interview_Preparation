// C++ 线程的使用
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>   // 条件变量
#include <queue>

using namespace std;

// 线程同步
// 经典案例：生产者 - 消费者 模型
queue<int> data_queue;  // 数据缓冲区
mutex mtx;      // 互斥锁，用于保护缓冲队列
condition_variable cv;      // 条件变量
bool isFinished = false;

// 生产者：生产数据
template<const int ITERATIONS>
void producer()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        this_thread::sleep_for(chrono::microseconds(500));

        // 代码块中：上锁保护缓冲区，并存入数据
        {
            lock_guard<mutex> lock(mtx);
            data_queue.push(i);
            cout << "[生产者]生产了数据: " << i << "\n";
        }

        // 通知其中一个消费者，单一数据生产完成
        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(mtx);
        isFinished = true;
    }

    // 通知所有消费者，生产结束
    cv.notify_all();
}

// 消费者：处理数据
// 数据队列非空，且生产未结束时，尝试消费数据
void consumer()
{
    while (true)
    {
        // 使用 unique_lock, 可手动上锁/解锁
        unique_lock<mutex> lock(mtx);
        /*
        * 使用 cv.wait() 控制消费者线程 唤醒/休眠
        * 1. lambda: 返回 false 时，释放锁并阻塞线程，进入休眠状态
        * 2. 收到 notify 信号时，唤醒线程
        * 3. 唤醒后重新获取锁，再次检查 lambda 条件
        */
        cv.wait(lock, [] { return !data_queue.empty() || isFinished; });

        if (data_queue.empty() && isFinished)
        {
            break;
        }

        /*
        * 【注意】如果两个函数不加锁，消费者线程会直接调用 data_queue.front()
        *         此时可能没有数据生产出来，导致调用失败
        */

        // 取出数据
        int data = data_queue.front();
        data_queue.pop();
        cout << "[消费者] 处理了数据: " << data << "\n";

        // 此处可手动解锁
        lock.unlock();

        // 模拟耗时
        this_thread::sleep_for(chrono::microseconds(200));
    }
}

int main()
{
    const int ITERATIONS = 5;

    thread t_prod(producer<ITERATIONS>);
    thread t_cons(consumer);

    t_prod.join();
    t_cons.join();

    std::cout << "All threads finished!!!\n";
}
