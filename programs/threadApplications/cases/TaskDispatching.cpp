// C++ 线程的使用
// 示例：实现“异步任务调度系统”，演示 shared_from_this() 如何延长对象的生命周期
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <functional>

using namespace std;

/*
 * 实现一个 “异步任务调度系统”，由任务类 Task 和调度器类 TaskDispatcher 组成
 * 任务类 Task:
 *      职责：封装具体的任务逻辑
 *      特征：使用 unique_ptr 管理，属于一次性资源（避免资源占用和多线程竞争）
 * 调度器类 TaskDispatcher:
 *      职责：接收任务、开辟异步线程、监控任务完成状态
 *      特征：继承自 enable_shared_from_this, 需要维持自身生命周期，直到回调函数安全执行完毕
 */

// 任务类：具体的业务逻辑
class Task
{
public:
    string name;

    Task(string n) : name(std::move(n))
    {
        cout << "[Task] " << name << " 被创建\n";
    }

    ~Task()
    {
        cout << "[Task] " << name << " 被销毁\n";
    }

    // 业务处理
    void execute()
    {
        cout << "[执行] 正在处理任务 " << name << "\n";
    }
};

// 调度器类：必须继承自 enable_shared_from_this
class TaskDispatcher : public std::enable_shared_from_this<TaskDispatcher>
{
public:
    TaskDispatcher()
    {
        cout << "[Dispatcher] 调度器已启动\n";
    }

    ~TaskDispatcher()
    {
        cout << "[Dispatcher] 调度器已关闭\n";
    }

    // 调度器的任务接收：通过 std::move(task) 从主线程接管任务
    void submit(unique_ptr<Task> task)
    {
        cout << "[Dispatcher] 调度器接收到任务 " << task->name << "\n";

        // 模拟异步操作
        // 【重点】“锁定”调度器的生命周期 -> 即使调度器对象被主线程销毁，调度器仍在内存中执行
        // 方法：使用 shared_from_this() 产生一个指向自己的 shared_ptr, 引用计数器不为 0 时，在内存中一直运行
        
        // 【坑1】不能直接使用 this 指针 -> 调度器对象被主线程销毁时，引发 this 指针悬挂
        // auto self = this;
        
        // 正确操作：使用 shared_from_this(), 生成指向调度器对象 .submit() 存储位置的 shared_ptr
        auto self = shared_from_this();

        // 创建子线程：使用一个 lambda 函数
        // 参数1 self: 子线程执行的函数，即调度器的 submit()
        // 参数2 t_ptr: 任务对象 task 的所有权
        thread t([self, t_ptr = std::move(task)]() mutable
            {
                // 模拟业务处理
                this_thread::sleep_for(chrono::seconds(1));

                t_ptr->execute();

                // 回调调度器的方法
                self->onTaskComplete(t_ptr->name);
            });

        // 线程分离
        t.detach();
    }

    void onTaskComplete(const string& name)
    {
        cout << "[Dispacther] 收到通知：任务 " << name << " 已完成\n";
    }
};

int main()
{
    {
        // 必须通过 shared_ptr 创建调度器，匹配 shared_from_this()
        std::shared_ptr<TaskDispatcher> dispatcher = std::make_shared<TaskDispatcher>();

        // 创建任务，每一个任务对象都使用 unique_ptr 管理
        // 通过 std::move(task) 传给调度器
        auto task1 = std::make_unique<Task>("数据加密");
        dispatcher->submit(std::move(task1));

        auto task2 = std::make_unique<Task>("日志上传");
        dispatcher->submit(std::move(task2));

        cout << "此时 dispatcher 的引用计数为: " << dispatcher.use_count() << "\n";

        cout << "<-------主线程即将离开作用域------->\n";
    }
    
    // 主线程作用域结束后，dispatcher 对象被销毁，dispatcher 引用计数减 1 但不为 0
    // 异步线程仍在执行时，dispachter 对象由于引用计数的存在而保留，供异步线程调用

    // 等待 2s, 获取异步执行结果
    this_thread::sleep_for(chrono::seconds(2));

    // main() 程序结束后，调度器彻底销毁
    cout << "<----------------------函数结束---------------------->\n";
}
