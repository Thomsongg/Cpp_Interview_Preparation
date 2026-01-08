// C++ 线程的使用
// 练习4：实现 “任务结果返回系统”
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>       // 包含 promise, future, packaged_task
#include <functional>
#include <queue>
#include <vector>
#include <string>
#include <syncstream>

using namespace std;

/*
 * 目标：实现一个“任务结果返回系统” -> 线程池核心
 * 包含三部分：(1) 任务打包；(2) 安全队列；(3) 执行系统
 * 结合 promise, future, packaged_task (线程-函数包装) 和之前实现过的 SafeQueue 类
 */

// 封装一个简单的任务包，存储一个返回结果的函数 Handle -> 使用 packaged_task
// std::packaged_task 可以自动关联 promise 和 future
struct AsyncTask
{
    // 执行时，自动调用内部的 promise.set_value()
    std::packaged_task<int()> task_func;
};

// 线程安全队列类（生产者-消费者模型）
class TaskQueue
{
private:
    // 【改进】数据缓存队列，存放任务包，而非单一数据
    queue<std::packaged_task<int()>> queue_;
    mutex mtx_;
    condition_variable cv_;
    bool isFinished_ = false;

public:
    // [生产者] 数据入队操作
    void push(std::packaged_task<int()> task)
    {
        {
            lock_guard<mutex> lock(mtx_);
            queue_.push(std::move(task));
        }
        cv_.notify_one();
    }

    // [生产者] 中止生产
    void stop()
    {
        lock_guard<mutex> lock(mtx_);

        isFinished_ = true;

        cv_.notify_all();
    }

    // [消费者] 获取数据并出队
    bool pop(std::packaged_task<int()>& task)
    {
        unique_lock<mutex> lock(mtx_);

        cv_.wait(lock, [this] { return !queue_.empty() || isFinished_; });

        if (queue_.empty() && isFinished_)
        {
            return false;
        }

        // 通过右值引用，直接给 task 赋值
        task = std::move(queue_.front());
        queue_.pop();

        return true;
    }
};

// 子线程函数
// 逻辑：通过 tq.pop() 获取任务并执行
void worker(TaskQueue& tq, int id)
{
    std::packaged_task<int()> cur_task;
    while (tq.pop(cur_task))
    {
        // 执行任务，packaged_task 会自动调用内部的 promise.set_value
        std::osyncstream(cout) << "[Worker " << id << "] 准备执行任务...\n";
        cur_task();
    }
    std::osyncstream(cout) << "[Worker " << id << "] 结束，准备下班!\n";
}

int main()
{
    // 执行逻辑：
    // 主线程派发任务，调用 taskQueue.push(task)
    // 子线程执行任务，通过 task 获取任务并执行

    // 公共的 TaskQueue 对象
    TaskQueue tq;

    // 启动两个子线程
    int num_thread = 2;
    vector<thread> pool;
    pool.reserve(num_thread);
    for (int i = 0; i < pool.capacity(); i++)
    {
        pool.emplace_back(worker, std::ref(tq), i + 1);
    }

    // 主线程异步派发任务，通过 future 对象，存储每个任务的执行结果（返回值） -> 存入 results[n]
    int num_task = 5;
    vector<std::future<int>> results;
    results.reserve(num_task);
    for (int i = 0; i < results.capacity(); i++)
    {
        // 在 std::packaged_task 中包装一个 lambda 定义的计算任务
        std::packaged_task<int()> task([i]()
            {
                this_thread::sleep_for(chrono::seconds(1));
                return i * i;
            });

        // 获取任务的“凭证”，即 future
        results.push_back(task.get_future());

        // 将任务存入缓存区
        std::osyncstream(cout) << "[主线程] 派发任务: 计算 " << i << " 的平方\n";
        tq.push(std::move(task));
    }

    // 主线程执行自己的任务
    std::osyncstream(cout) << "[主线程] 完成任务派发，开始处理其他业务...\n";

    for (int i = 0; i < results.size(); i++)
    {
        // 调用 .get() 方法，阻塞主线程并获取 future 内的结果
        int val = results[i].get();
        std::osyncstream(cout) << "任务 " << i + 1 << " 的结果是: " << val << "\n";
    }

    // 使用专属方法结束线程，优雅关闭
    tq.stop();
    for (auto& t : pool)
    {
        t.join();
    }

    std::osyncstream(cout) << "所有线程结束!!!\n";
}
