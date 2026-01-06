// C++ 线程的使用
// 练习3：哲学家进餐问题（死锁实战）
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <syncstream>   // C++20 引入 std::osyncstream

using namespace std;

/*
 * 背景：5 人围坐一圈，每两人之间有一根筷子，需要同时拿到左右两根筷子
 * 目标：
 *  1. 用 5 个 mutex 代表 5 根筷子
 *  2. 创建 5 个线程代表 5 个人
 *  3. 制造死锁：每人先拿左边筷子，再拿右边
 *  4. 打破死锁：使用 std::scoped_lock 同时获取两根筷子
 */

// 【推荐1】mutex 数组及其他公共变量，由参数传递给线程函数，而非声明为全局变量
// 【推荐2】引入 <syncstream>(C++20)，使用 std::osyncstream(std::cout) 代替 cout，实现安全化文本输出

// 假设编号由顺时针增加，哲学家左边的锁 id 与自身相同
// 哲学家 id 和 锁 id 的对应关系: id -> id, (id + 1) % 5
void philosopher_deadlock(int person_id, vector<mutex>& chopsticks, int num_philosopher)
{
    int left_chop_id = person_id;
    int right_chop_id = (person_id + 1) % num_philosopher;

    while (true)
    {
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 正在思考...\n ";

        // 先获取左侧的锁
        lock_guard<mutex> lock1(chopsticks[left_chop_id]);
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 拿到左边的筷子 " << left_chop_id << "...\n";

        // 短暂停顿，增加死锁发生的概率
        this_thread::sleep_for(chrono::microseconds(10));

        // 所有线程发现右边的锁被占用，此时就会发生死锁，无限期阻塞当前进程...

        // 再尝试获取右边的锁
        lock_guard<mutex> lock2(chopsticks[right_chop_id]);
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 拿到右边的筷子 " << right_chop_id << "...\n";

        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 可以开吃了!\n";

        this_thread::sleep_for(chrono::microseconds(100));
    }
}

// 【打破死锁】使用 std::scoped_lock 打破死锁
void philosopher_safe(int person_id, vector<mutex>& chopsticks, int num_philosopher)
{
    int left_chop_id = person_id;
    int right_chop_id = (person_id + 1) % num_philosopher;

    while (true)
    {
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 正在思考...\n ";

        // 【改进】使用 scoped_lock, 尝试同时获取两边的锁
        // 原理：如果拿不到全部的锁，会释放并重试；
        // 从而打破死锁的一个关键条件：“请求与保持”
        scoped_lock lock(chopsticks[left_chop_id], chopsticks[right_chop_id]);
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 拿到左边的筷子 " << left_chop_id << "...\n";

        this_thread::sleep_for(chrono::microseconds(100));

        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 拿到右边的筷子 " << right_chop_id << "...\n";
        std::osyncstream(cout) << "哲学家 " << person_id + 1 << " 可以开吃了!\n";

        this_thread::sleep_for(chrono::microseconds(100));
    }
}

int main()
{
    // mutex 数组需直接初始化，
    int num_threads = 5;
    vector<mutex> chopsticks(num_threads);
    vector<thread> philosophers;

    // 【坑1】互斥锁 mutex 无法拷贝 (copy) 和移动 (move)
    // 错误代码：
    //for (int i = 0; i < num_threads; i++)
    //{
    //    mutex mtx;
    //    vec_mtx.emplace_back(mtx);
    //}


    for (int i = 0; i < num_threads; i++)
    {
        // 【注意】传入的 vector<mutex> 是引用
        philosophers.emplace_back(philosopher_safe, i, std::ref(chopsticks), num_threads);
    }

    for (auto& t : philosophers)
    {
        t.join();
    }

    std::osyncstream(cout) << "All philosophers finished!!!\n";
}
