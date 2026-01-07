// 智能指针 & RAII
#include <iostream>
#include <memory>

using namespace std;

/*
 * 示例2: 使用 shared_ptr 和 weak_ptr 模拟双向链表
 * 目标：实现每个节点的“前驱”和“后继”，并避免循环引用
 * 要点：使用 shared_ptr 前驱，weak_ptr 后继
 */

struct Node
{
    int value;
    std::shared_ptr<Node> next;     // 向后强引用，保持对象存活
    std::weak_ptr<Node> prev;       // 向前弱引用，作观察，不计数

    // 节点的构造
    Node(int val) : value(val)
    {
        cout << "Node " << value << " created.\n";
    }

    ~Node()
    {
        cout << "Node " << value << " destroyed.\n";
    }
};

// 双向链表的使用
void listDemo()
{
    // 创建两个节点
    auto head = make_shared<Node>(1);
    auto second = make_shared<Node>(2);

    // 建立连接
    head->next = second;
    second->prev = head;

    /*
     * 【注意】如果 prev 为 shared_ptr, 会导致循环引用（head->next 与 second->prev）
     *         作用域结束后，head 与 second 计数不会到 0，两个节点会永久存在，导致内存泄漏
     */

    // 打印引用计数
    cout << "Head ref count: " << head.use_count() << "\n";
    cout << "Second ref count: " << second.use_count() << "\n";

    /*
     * 【注解】此时 second->prev 为弱指针，只作观察，不会增加 head (shared_ptr) 的计数
     *         结果：Head 计数为 1，Second 计数为 2
     */

    // prev 为弱指针，可使用 .lock() 检查前驱节点 head 是否存活
    if (auto p = second->prev.lock())
    {
        cout << "Node second's previous is " << p->value << "\n";
    }
}

int main()
{
    listDemo();

    std::cout << "-----函数已结束-----\n";
}
