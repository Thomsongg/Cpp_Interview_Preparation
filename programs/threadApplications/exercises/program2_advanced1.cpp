// C++ 线程的使用
// 练习2：线程安全的“任务队列”（模拟线程池基础）
// 使用“毒药药丸”模式，控制消费者线程中断
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <random>       // 随机数生成库
#include <optional>     // 选项库

using namespace std;

// 演示“毒药药丸”模式，需要缓存区可以存放 optional(nullopt)
// 数据缓存区 data_queue 更名为 任务缓存区 task_queue
queue<optional<int>> task_queue;
condition_variable cv;
mutex mtx;

// 生产者：每隔 100ms 生成一个随机数并存入缓存区
void producer(const int iterations, const int num_consumers)
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
        task_queue.push(random_num);
        cout << "[生产者]" << this_thread::get_id() << "生产了一个数据:" << random_num << "...\n";

        // 通知消费者其一
        cv.notify_one();
    }
    
    // 数据全部生产完成，投递“毒药”(nullopt)
    {
        lock_guard<mutex> lock(mtx);
        
        // 提前计算好消费者数量，生产对应数目的“毒药”
        for (int i = 0; i < num_consumers; i++)
        {
            task_queue.push(nullopt);
        }
        cout << "[生产者]" << this_thread::get_id() << "全部数据生产完成!\n";
    }

    cv.notify_one();
}

// 消费者：持续从缓存区取出数据并打印
void consumer(const int iterations)
{
    while (true)
    {
        // 上锁：阻止其他线程争抢 缓存区 和 结束标志位
        // 使用 unique_lock: 可以手动加解锁
        unique_lock<mutex> lock(mtx);

        // 通过条件变量，等待生产者唤醒
        // 此时无需判断标志位
        cv.wait(lock, [] { return !task_queue.empty(); });

        // 【重要】消费者线程的退出
        // (1) 传统方法：标志位判断
        //if (task_queue.empty() && isFinished)
        //{
        //    cout << "[消费者]" << this_thread::get_id() << " 结束!\n";
        //    break;
        //}

        // (2) “毒药药丸”模式
        auto task = task_queue.front();
        task_queue.pop();

        // 获取到“毒药” nullopt 时退出
        if (!task.has_value())
        {
            cout << "[消费者]" << this_thread::get_id() << " 捕获到结束信号，现在退出!\n";
            break;
        }

        cout << "[消费者]" << this_thread::get_id() << " 获取到数据: " << task.value() << "\n";

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
    // 这里为了简化模型，采用单生产者模式
    thread t_prod1(producer, iterations, 2);
    //thread t_prod2(producer, iterations);
    //thread t_prod3(producer, iterations);
    thread t_cons1(consumer, iterations);
    thread t_cons2(consumer, iterations);

    t_prod1.join();
    //t_prod2.join();
    t_cons1.join();
    //t_prod3.join();
    t_cons2.join();

    std::cout << "All threads finished!!!\n";
}
