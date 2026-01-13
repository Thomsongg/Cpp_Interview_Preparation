// 多线程代码 “找茬”
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

std::vector<int> sharedData;
std::mutex dataMutex;
int counter = 0;  // 问题点1
bool ready = false;
std::condition_variable cv;     // 新增条件变量，实现跨线程同步

void producer() {
    for (int i = 0; i < 1000; i++) {
        // 【问题点】counter++ 非原子操作，也没有上锁，容易引起线程竞争
        sharedData.push_back(i);
        counter++;  // 问题点3
    }
    // 【问题点1】变量 ready 涉及多线程同步，可以使用条件变量 cv 或 promise-future
    // 【问题点2】编译器优化可能导致指令重排，会将 ready = true 放在循环之前，影响消费者线程访问 sharedData!!!
    ready = true;  // 问题点4：没有同步
}

// 【修改1】生产者、消费者上锁，使用 condition_variable 同步
void producer_safe()
{
    for (int i = 0; i < 1000; i++)
    {
        // 增加延时
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        std::lock_guard<std::mutex> lock(dataMutex);
        sharedData.push_back(i);
        counter++;
    }

    {
        std::lock_guard<std::mutex> lock(dataMutex);
        ready = true;
    }

    // 通知所有其他线程，实现同步
    cv.notify_all();
}

void consumer() {
    // 【问题点1】未上锁，直接读取 ready，可能会与消费者线程竞争
    // 【问题点2】直接使用 while 判断，会忙等待；推荐使用 cv 唤醒
    while (!ready) {  // 问题点5：忙等待
        // 等待数据
    }

    int sum = 0;

    // 【问题点】跨线程访问同一个数组 sharedData 时，数据不同步
    //  问题1: 生产者线程函数编译时可能引起指令重排，导致消费者先读到 ready == true, 此时 sharedData 未更新，可能会访问到无效元素或越界
    //  问题2: sharedData 在生产者修改后可能保存在 CPU 缓存中（内存没有同步修改），导致消费者线程读取的数据不一致
    //  问题3: 数组的 push_back 非原子操作，扩容时会重新分配内存，导致此时的 size 可能并非实际元素数（消费者获取的可能是中间状态）
    // 【解决方案】线程同步
    for (int i = 0; i < sharedData.size(); i++) {  // 问题点6：可能越界
        sum += sharedData[i];
    }
    std::cout << "Sum: " << sum << std::endl;
}

void consumer_safe()
{
    // 涉及的两个操作都要上锁：等待数据、访问数据
    // 消费者线程可使用 unique_lock, 手动解锁
    std::unique_lock<std::mutex> lock(dataMutex);

    // 唤醒线程
    cv.wait(lock, [] { return ready; });

    // 在读取到 ready == true 时，最好有一定的时延，等待 sharedData 更新完成
    // 如果使用条件变量唤醒，可不加时延
    std::this_thread::sleep_for(std::chrono::microseconds(200));

    // 访问数据
    // 【重要】如果不使用 cv 跨线程同步，数组 sharedData 在生产者和消费者中的状态可能不一致
    int sum = 0;
    for (int i = 0; i < sharedData.size(); i++) {
        sum += sharedData[i];
    }
    std::cout << "Sum: " << sum << std::endl;
}

class ThreadSafeQueue {
    // 【问题点】线程竞争：对象 tsq 被多个线程同时访问
    // 【修改】加锁
    std::vector<int> queue;
    std::mutex mtx;

public:
    // 【修改】所有对数组 queue 的访问都要上锁
    void push(int value) {
        // push_back 操作上锁
        std::lock_guard<std::mutex> lock(mtx);
        queue.push_back(value);
    }

    int pop() {
        // 读取 queue 和 erase 操作上锁
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return -1;

        // 【优化】可使用移动语义传给 value
        //int value = queue.front();
        int value = std::move(queue.front());
        queue.erase(queue.begin());
        return value;
    }
};

void raceConditionDemo() {
    // 构建线程池
    ThreadSafeQueue tsq;
    int sum = 0;

    // 生产线程
    auto worker = [&tsq]() {
            for (int i = 0; i < 100; i++) {
                tsq.push(i);
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        };

    // 消费线程：持续出栈
    auto worker2 = [&tsq, &sum]()
        {
            // 先延时，待一部分数据生产后再消费
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            while (tsq.pop() != -1)
            {
                sum += tsq.pop();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout << "Final sum:" << sum << "\n";
        };

    std::thread t1(worker);
    std::thread t2(worker2);

    t1.join();
    t2.join();

    std::cout << "Queue operations completed\n";
}

void deadlockExample() {
    std::mutex mutex1, mutex2;

    // 【问题点】线程获取锁的顺序不合理
    // mutex1/mutex2 被某个线程永久占有，另一个线程无限等待
    
    //auto task1 = [&]() {
    //    std::lock_guard<std::mutex> lock1(mutex1);
    //    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //    std::lock_guard<std::mutex> lock2(mutex2);  // 问题点7：可能死锁
    //    std::cout << "Task1 completed\n";
    //    };

    //auto task2 = [&]() {
    //    std::lock_guard<std::mutex> lock2(mutex2);
    //    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //    std::lock_guard<std::mutex> lock1(mutex1);  // 问题点8：可能死锁
    //    std::cout << "Task2 completed\n";
    //    };

    // 【修改1】传统方法：按固定顺序上锁
    auto task1_safe = [&]()
        {
            std::lock_guard<std::mutex> lock1(mutex1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::lock_guard<std::mutex> lock2(mutex2);
            std::cout << "Task1 completed\n";
        };

    auto task2_safe = [&]()
        {
            std::lock_guard<std::mutex> lock1(mutex1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::lock_guard<std::mutex> lock2(mutex2);
            std::cout << "Task2 completed\n";
        };

    // 【修改2】C++17 现代做法: 使用 scoped_lock 同时上多个锁
    auto task1_modern = [&]()
        {
            std::scoped_lock(mutex1, mutex2);
            std::cout << "Task1 completed\n";
        };

    auto task2_modern = [&]()
        {
            std::scoped_lock(mutex1, mutex2);
            std::cout << "Task2 completed\n";
        };

    std::thread t1(task1_modern);
    std::thread t2(task2_modern);

    t1.join();
    t2.join();
}

int main() {
    //std::thread producerThread(producer);
    //std::thread consumerThread(consumer);

    //producerThread.join();
    //consumerThread.join();

    //std::cout << "Final counter value: " << counter << std::endl;

    //raceConditionDemo();
    deadlockExample();

    return 0;
}
