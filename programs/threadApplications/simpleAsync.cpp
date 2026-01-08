// C++ 线程的使用
#include <thread>
#include <mutex>
#include <iostream>
#include <future>
#include <chrono>

using namespace std;

/*
 * 跨线程的任务协作——异步模式
 * 基于 std::promise （发送端） 和 std::future（接收端） 实现
 * 场景：类 “生产者-消费者”模式
 * (1) 子线程执行复杂的计算任务时，主线程执行本职工作（不卡死）； (2) 直到子线程任务完成，主线程拿到结果
 */

// 子线程函数 -> 异步执行
void calculateSum(std::promise<int> result_promise, int a, int b)
{
    cout << "[子线程]正在执行计算...\n";

    this_thread::sleep_for(chrono::seconds(2));

    int sum = a + b;

    // 【异步管道】将结果存入 promise, 则另一端 future 变为就绪状态
    result_promise.set_value(sum);
    cout << "[子线程]计算结束，结果已存入管道。\n";
}

int main()
{
    // 创建 promise 对象，并将 future 与 promise 相关联，实现管道
    std::promise<int> sum_promise;

    // promise_ins.get_future() 方法，表示 future 期待获取 promise 中的 int
    std::future<int> sum_future = sum_promise.get_future();

    // 启动子线程
    // 注意 promise 对象必须使用移动语义传入，无法拷贝构造
    thread subThread(calculateSum, std::move(sum_promise), 10, 20);

    // 此过程不会阻塞主线程
    // 模拟主线程的任务
    cout << "[主线程]执行本职工作，未被阻塞。\n";
    for (int i = 0; i < 3; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "[主线程]忙碌中...\n";
    }

    // 主线程尝试获取结果
    // 如果子线程未结束，调用 .get() 会阻塞主线程，直到拿到 promise 的结果
    cout << "[主线程]等待结果中...\n";
    int final_ret = sum_future.get();

    cout << "[主线程]拿到最终结果: " << final_ret << "\n";

    // 等待子线程结束
    subThread.join();

    std::cout << "所有线程结束!!!\n";
}
