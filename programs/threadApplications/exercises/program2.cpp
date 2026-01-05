// C++ 线程的使用
// 练习2：线程安全的“任务队列”（模拟线程池基础）
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <random>   // 随机数生成库

using namespace std;

// 目标：实现一个简单的 生产者-消费者 模型
// 核心：线程同步
// 要求：缓存区为空时，消费者线程休眠，不能忙等；生产者放入数据后要唤醒消费者

/*
* 全局变量：
* 1. 缓存区(queue): 存放数据
* 2. 条件变量: 生产者唤醒消费者(cv.notify_xxx)、消费者等待被唤醒(cv.wait)
* 3. 数据生产结束标志位 isFinished
* 4. 线程锁: 保护缓存区数据存放、生产结束标志位修改、消费者取出数据后解锁(unique_lock)
*/
queue<int> data_queue;
condition_variable cv;
bool isFinished = false;
mutex mtx;

// 生产者：每隔 100ms 生成一个随机数并存入缓存区
void producer(const int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        // 时间间隔
        this_thread::sleep_for(chrono::milliseconds(100));

        // 生成随机数（1-100 的整数），并存入缓存区
        random_device rd;       // 随机设备
        mt19937 gen(rd());      // 随机数引擎
        uniform_int_distribution<> dis(1, 100);         // 定义分布范围
        int random_num = dis(gen);      // 创建随机数

        // 上锁再存放数据
        lock_guard<mutex> lock(mtx);
        data_queue.push(random_num);
        cout << "[生产者]" << this_thread::get_id() << "生产了一个数据:" << random_num << "...\n";

        // 通知消费者其一
        cv.notify_one();
    }
    
    // 数据全部生产完成
    {
        lock_guard<mutex> lock(mtx);
        isFinished = true;
        cout << "[生产者]" << this_thread::get_id() << "全部数据生产完成!\n";
    }

    // 通知全部消费者
    cv.notify_all();
}

// 消费者：持续从缓存区取出数据并打印
void consumer(const int iterations)
{
    while (true)
    {
        // 上锁：阻止其他线程争抢 缓存区 和 结束标志位
        // 使用 unique_lock: 可以手动加解锁
        unique_lock<mutex> lock(mtx);

        // 通过条件变量，等待生产者唤醒，唤醒条件满足下方其一
        // (1) 数据缓存区非空，才能消费
        // (2) 生产结束
        cv.wait(lock, [] { return !data_queue.empty() || isFinished; });

        // 【重要】消费者线程的退出
        if (data_queue.empty() && isFinished)
        {
            cout << "[消费者]" << this_thread::get_id() << " 结束!\n";
            break;
        }

        // 从缓存区队头获取数据，并打印
        // 【注意】获取数据后必须出队!!!
        int data = data_queue.front();
        data_queue.pop();
        cout << "[消费者]" << this_thread::get_id() << " 获取到数据: " << data << "\n";

        // 手动解锁，解除占用
        lock.unlock();

        this_thread::sleep_for(chrono::microseconds(50));
    }
}

int main()
{
    const int iterations = 5;
    int num_threads = 5;

    // 创建生产者和消费者线程
    thread t_prod1(producer, iterations);
    thread t_prod2(producer, iterations);
    thread t_prod3(producer, iterations);
    thread t_cons1(consumer, iterations);
    thread t_cons2(consumer, iterations);

    t_prod1.join();
    t_prod2.join();
    t_cons1.join();
    t_prod3.join();
    t_cons2.join();

    std::cout << "All threads finished!!!\n";
}
