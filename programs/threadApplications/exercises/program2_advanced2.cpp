// C++ 线程的使用
// 练习2：线程安全的“任务队列”（模拟线程池基础）
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <queue>
#include <random>       // 随机数生成库
#include <string>       // 用于封装类打印特定字符串
#include <sstream>      // 使用 oss 拼接字符串

using namespace std;

// 工程化处理：将线程相关操作（数据入队、互斥锁、线程结束等）封装到特定类
// 用法：在 main 统一声明一个公共实例，由所有生产者、消费者线程共用
// 功能：封装数据生产和消费的所有操作
template<typename T>
class SafeQueue
{
private:
    queue<int> queue_;
    mutex mtx_;
    mutex cout_mtx;    // 信息输出专属锁，避免多个线程同时调用 cout, 导致输出产生“信道交织”
    condition_variable cv_;
    bool isFinished_ = false;

public:
    // 数据入队和设置结束标志位操作
    void push(T val, int thread_id)
    {
        lock_guard<mutex> lock(mtx_);

        safe_log_action("生产者", thread_id, "生产了一个随机数", val);

        // 【重要】此处使用 std::move，避免值传递开销，且适配任意数据类型
        queue_.push(std::move(val));
        
        // 通知消费者获取数据
        cv_.notify_one();
    }

    void setFinished(int thread_id)
    {
        lock_guard<mutex> lock(mtx_);

        isFinished_ = true;

        ostringstream oss;
        oss << "生产者 " << thread_id << " 任务完成，结束生产！";

        safe_log_msg(oss.str());

        // 通知所有消费者
        cv_.notify_all();
    }

    // 获取任务并弹出
    // 消费者线程可使用返回值控制线程结束
    bool tryPop(T& value, int thread_id)
    {
        unique_lock<mutex> lock(mtx_);

        // 等待唤醒
        cv_.wait(lock, [this] { return !queue_.empty() || isFinished_; });

        // 控制线程结束
        ostringstream oss;
        oss << "消费者 " << thread_id << " 线程结束...";

        if (queue_.empty() && isFinished_)
        {
            safe_log_msg(oss.str());
            return false;
        }

        // 获取数据并尝试弹出
        // 使用 std::move 避免值传递
        value = std::move(queue_.front());
        queue_.pop();
        safe_log_action("消费者", thread_id, "获取到数据:", value);

        return true;
    }

    // 原子化打印方法，输出完整的线程操作内容
    // 包含：tag、线程 id、执行动作、值
    // 示例：[生产者 0] 生产了一个随机数 89...
    void safe_log_action(const string& tag, int id, const string& action, int value)
    {
        // 上锁，保护 cout
        lock_guard<mutex> lock(cout_mtx);

        cout << "[" << tag <<" id" << "] " << action << " " << value << "...\n";
    }

    // 普通的原子化打印方法
    void safe_log_msg(const string& msg)
    {
        lock_guard<mutex> lock(cout_mtx);

        cout << msg << "\n";
    }
};

// 生产者：每隔 100ms 生成一个随机数并存入缓存区
void producer(SafeQueue<int>& q_instance, int id, const int iterations, int interval)
{
    for (int i = 0; i < iterations; i++)
    {
        // 生成随机数
        // 【TODO】随机数引擎需要上锁
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 100);
        int random_num = dis(gen);

        q_instance.push(random_num, id);

        // 等待特定时间
        this_thread::sleep_for(chrono::microseconds(interval));
    }

    q_instance.setFinished(id);
}

// 消费者：持续从缓存区取出数据并打印
void consumer(SafeQueue<int>& q_instance, int id, int interval)
{
    int value;

    // 现在可直接通过 SafeQueue 的方法，控制线程中断
    while (q_instance.tryPop(value, id))
    {
        this_thread::sleep_for(chrono::microseconds(interval));
    }
}

int main()
{
    SafeQueue<int> public_instance;
    int nums_prod = 3;
    int nums_cons = 2;
    const int iterations = 5;
    int interval_ms_prod = 100;
    int interval_ms_cons = 50;

    // 分别创建生产者、消费者线程
    vector<thread> threads_prod;
    vector<thread> threads_cons;

    for (int i = 0; i < nums_prod; i++)
    {
        threads_prod.emplace_back(producer, std::ref(public_instance), i + 1, iterations, interval_ms_prod);
    }

    for (int i = 0; i < nums_cons; i++)
    {
        threads_cons.emplace_back(consumer, std::ref(public_instance), i + 1, interval_ms_cons);
    }

    for (int i = 0; i < nums_prod; i++)
    {
        threads_prod[i].join();
    }

    for (int i = 0; i < nums_cons; i++)
    {
        threads_cons[i].join();
    }

    std::cout << "All threads finished!!!\n";
}
