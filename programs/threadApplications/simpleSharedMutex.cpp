// C++ 线程的使用
#include <thread>
#include <shared_mutex>     // 读写锁 (std::shared_mutex, C++17)
#include <iostream>
#include <vector>

using namespace std;

// 读写锁 (C++17) -> 用于读多写少的场景
// 读锁：不互斥
// 写锁：互斥
int shared_data = 0;
shared_mutex rw_mtx;

// 读者：只读操作，上读锁
void readData(int id)
{
    shared_lock<shared_mutex> lock(rw_mtx);
    cout << "读者 " << id << " 正在读取数据: " << shared_data << "\n";
    this_thread::sleep_for(chrono::milliseconds(10));
}

// 写者：写操作，使用 unique_lock 上锁
void writeData(int val)
{
    // 当读锁释放后，才可以上写锁
    unique_lock<shared_mutex> lock(rw_mtx);
    shared_data = val;
    cout << "写者修改数据为: " << shared_data << "\n";
    this_thread::sleep_for(chrono::milliseconds(20));
}

int main()
{
    vector<thread> threads;

    // 读写线程交叉
    threads.emplace_back(readData, 1);
    threads.emplace_back(readData, 2);

    // 写者线程会阻塞后续读者线程，直到写操作完成
    threads.emplace_back(writeData, 100);

    threads.emplace_back(readData, 3);
    threads.emplace_back(readData, 4);

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "All threads finished!!!\n";
}
