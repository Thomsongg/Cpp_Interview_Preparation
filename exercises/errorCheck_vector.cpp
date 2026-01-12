#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <chrono>

class Person {
    std::string name;
    int age;

public:
    Person() : name("Unknown"), age(0) {
        std::cout << "Default constructor\n";
    }

    Person(const std::string& n, int a) : name(n), age(a) {
        std::cout << "Constructor: " << name << "\n";
    }

    Person(const Person& other) : name(other.name), age(other.age) {
        std::cout << "Copy constructor: " << name << "\n";
    }

    Person(Person&& other) noexcept : name(std::move(other.name)), age(other.age) {
        std::cout << "Move constructor: " << name << "\n";
    }

    ~Person() {
        std::cout << "Destructor: " << name << "\n";
    }

    std::string getName() const { return name; }
    int getAge() const { return age; }
};

void vectorOperations() {
    std::cout << "=== 1. 基本操作与容量问题 ===\n";
    std::vector<int> vec;

    // 问题点：请找出
    // 【问题点】数组 vector 未进行初始化，直接使用索引访问会导致越界
    // 【修改】初始化 或 扩容并填充后，再访问
    //for (int i = 0; i < 5; i++) {
    //    vec[i] = i;
    //}

    // 问题点：请找出
    // 【问题点】vec.reserve(n) 只是预留空间，没有填充内容, vec.size() 为 0
    vec.reserve(10);
    std::cout << "After reserve(10), size = " << vec.size()
        << ", capacity = " << vec.capacity() << "\n";

    // 问题点：请找出
    // 【问题点】reserve 预留空间，如果没有添加新的对象，不能使用索引
    //  即使用索引访问的前提：vector 内已有对象，且访问的索引不能越界
    /*for (int i = 0; i < 10; i++) {
        vec[i] = i * 2;
    }*/
    
    // 正确用法：使用 push_back / insert 插入对象
    // 之后才可以使用索引
    for (int i = 0; i < 10; i++)
    {
        vec.emplace_back(i * 2);
    }

    std::cout << "After push_back, size = " << vec.size()
        << ", capacity = " << vec.capacity() << "\n";
}

// vector 迭代器的使用
void iteratorProblems() {
    std::cout << "\n=== 2. 迭代器失效问题 ===\n";
    std::vector<int> nums = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    nums.push_back(11);

    // 问题点：请找出
    // 【问题点】插入/删除 导致的迭代器失效问题
    //  每次执行 nums.erase(it) 操作，后续元素前移，原始的迭代器失效
    //  可以使用 it = nums.erase(it) 返回新的迭代器

    // 注释掉问题代码：会导致迭代器访问越界
    /*for (auto it = nums.begin(); it != nums.end(); it++) {
        if (*it % 2 == 0) {
            nums.erase(it);
        }
    }*/

    // 正确方法如下
    //for (auto it = nums.begin(); it != nums.end();)
    //{
    //    if (*it % 2 == 0)
    //    {
    //        it = nums.erase(it);
    //    }
    //    else
    //    {
    //        // 不删除时，后续元素不会前移，直接 it++
    //        it++;
    //    }
    //}

    //nums.clear();
    //nums = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    // 指定迭代器 -> middle = 0 + 10 / 2 == 5 -> *middle = nums[5] = 6
    std::vector<int>::iterator middle = nums.begin() + nums.size() / 2;
    std::cout << "Middle element: " << *middle << "\n";

    // 问题点：请找出
    // 【重要】在 VS 上采用 1.5 倍扩容
    for (int i = 0; i < 10; i++) {
        nums.push_back(i);
    }

    // 【问题点】插入元素 或 尾插导致扩容时，原有迭代器失效
    //  报错："can't dereference invalidated vector iterator"
    //std::cout << "Middle element after push_back: " << *middle << "\n";

    // 使用索引，删除所有的‘2’
    // 问题点：请找出
    nums = { 1, 2, 2, 3, 2, 4, 2, 5 };
    std::cout << "Original vector of nums:\n";
    for (auto& num : nums)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";

    // 【问题点】erase 操作会引起后续元素前移，导致索引错位
    /*for (size_t i = 0; i < nums.size(); i++) {
        if (nums[i] == 2) {
            nums.erase(nums.begin() + i);
        }
    }*/

    // 【改进1】使用 remove_if 删除指定条件的元素（现代方法）
    // 用法：结合 erase 使用，通过 lambda 提供删除的条件 -> nums.erase(std::remove_if(nums.begin(), nums.end(), lambda), nums.end())
    nums.erase(std::remove_if(nums.begin(), nums.end(), [](int n) {return n == 2;}), nums.end());

    std::cout << "After erased:\n";
    for (auto& num : nums)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";

    // 【改进2】从前向后遍历，不删除时 i++ （同迭代器删除法，不做赘述）

    std::cout << "After removing 2s: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";
}

void emplaceVsPush() {
    std::cout << "\n=== 3. emplace_back 与 push_back 区别 ===\n";
    std::vector<Person> people;

    Person alice("Alice", 30);
    //Person* Tommy = new Person("Tommy", 25);

    // 问题点：请找出
    // 【问题点】emplace_back 的参数为已构造的对象时，不如使用 push_back
    //  问题1：语义混淆 -> emplace_back 旨在原地构造，而非直接传递一个构造好的对象（过程与 push_back 相同）
    //  问题2：性能与 push_back(alice)相同，先拷贝到临时内存，再复制到 people
    //  性能排序（从小到大）： emplace_back(alice) == push_back(alice) < push_back(Person("Alice", age)) < push_back(std::move(alice)) < emplace_back("Alice", age)
    people.emplace_back(alice);     // 可运行但不是性能最优

    // 【修改1】使用 push_back 拷贝
    people.push_back(alice);

    // 【修改2】原对象是临时对象时，建议使用移动传参
    people.push_back(std::move(alice));

    // 问题点：请找出
    // 【问题点】临时构造的对象，使用 emplace_back 更高效
    people.push_back(Person("Bob", 25));        // push_back: 先构造对象，值传递时又构造了一遍（两次构造），低效
    people.emplace_back(Person("Daniel", 20));  // emplace_back: 原地构造

    // emplace_back 的另一种传参方式：直接传入 args -> people 中对象的构造参数
    people.emplace_back("Charlie", 35);

    std::cout << "\nPeople in vector:\n";
    for (const auto& p : people) {
        std::cout << p.getName() << " (" << p.getAge() << ")\n";
    }
}

void capacityManagement() {
    std::cout << "\n=== 4. 容量管理与内存释放 ===\n";
    std::vector<int> data;

    //auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++) {
        data.push_back(i);
    }

    //auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "未预留空间，耗时: " << duration.count() << "ms\n";

    std::cout << "After adding 1000 elements:\n";
    std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";

    // 只保留前 100 个元素，其他全部删除
    data.erase(data.begin() + 100, data.end());

    std::cout << "After erasing 900 elements:\n";
    std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";

    // 【重要】如何将 capacity 缩小至与 size 相等？
    // 【方法1】shrink_to_fit (现代 C++11)
    data.shrink_to_fit();
    std::cout << "After shrink_to_fit:\n";
    std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";

    // 【方法2】按当前对象创建临时对象，并调用 swap(data); 传统方法，同样有效
    //std::vector<int>(data).swap(data);
    //std::cout << "After swap to original data:\n";
    //std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";

    // 问题点：请找出
    // 【问题点】clear 方法只会清空值，不会销毁空间
    // 【修改】使用 swap 方法
    data.clear();
    std::cout << "After clear:\n";
    std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";

    // 【方法1】clear + shrink_to_fit (C++ 11)
    //data.shrink_to_fit();

    // 【方法2】临时变量交换
    //std::vector<int>().swap(data);

    // 问题点：请找出
    // 【问题点】引入了临时变量 emptyVec, 推荐使用 std::vector<int>().swap(data)
    std::vector<int> emptyVec;
    data.swap(emptyVec);
    std::cout << "After swap with empty vector:\n";
    std::cout << "Size: " << data.size() << ", Capacity: " << data.capacity() << "\n";
}

void algorithmIssues() {
    std::cout << "\n=== 5. 算法与vector配合问题 ===\n";
    std::vector<int> numbers = { 5, 2, 8, 1, 9, 3 };

    // 问题点：请找出
    // 【问题点】remove 并没有删除，numbers.size 并没有改变 -> 迭代器范围内最后一位的元素补位
    // 新的 numbers = {5, 8, 1, 9, 3, 3} 
    auto new_end = std::remove(numbers.begin(), numbers.end() - 2, 2);

    // 【思考】如果 new_end = std::remove(numbers.begin(), numbers.end() - 2, 2), 结果是什么？
    //  新的 numbers = {5, 8, 1, 1, 9, 3}, remove 范围的最后一个元素 1 补位

    std::cout << "After std::remove(2):\n";
    std::cout << "Size: " << numbers.size() << "\n";
    std::cout << "Elements: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    // 问题点：请找出
    // 【问题点】remove + erase, erase(new_end, numbers.end()) 操作会将 new_end 后的元素全部删除
    //numbers.erase(new_end, numbers.end());
    // 【修改】只删除迭代器所在的元素
    numbers.erase(new_end);

    std::cout << "After erase-remove:\n";
    std::cout << "Size: " << numbers.size() << "\n";
    std::cout << "Elements: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    // 排序操作: 实现有序性
    std::sort(numbers.begin(), numbers.end());

    std::cout << "After sort:\n";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";


    // 插入新元素：破坏了有序性
    numbers.push_back(4);

    // 问题点：请找出
    // 【问题点】插入后破坏了有序性
    if (std::is_sorted(numbers.begin(), numbers.end())) {
        std::cout << "Vector is still sorted\n";
    }

    // 即使插入的元素保持了原来的顺序，数组的有序性仍被破坏 -> 因为迭代器失效
    numbers.push_back(10);
    if (std::is_sorted(numbers.begin(), numbers.end())) {
        std::cout << "Vector is still sorted\n";
    }

    // 问题点：请找出
    // 【问题点】数组 dest 未预留空间和初始值，直接调用 copy 运行时报错
    std::vector<int> source = { 1, 2, 3, 4, 5 };
    std::vector<int> dest;

    // 【修改】预留空间并赋初值再操作
    // 【错误1】只预留空间
    //dest.reserve(100); -> 仍会报错

    // 正确做法：resize, 预留空间并赋初值
    dest.resize(5);     // 或 dest.resize(5, 0)

    std::copy(source.begin(), source.end(), dest.begin());

    std::cout << "After copy, dest is:\n";
    for (int n : dest) std::cout << n << " ";
    std::cout << "\n";
}

void performanceTraps() {
    std::cout << "\n=== 6. 性能陷阱 ===\n";

    std::vector<int> vec;

    // 问题点：请找出
    // 【问题点】多次尾插，引发多次扩容，可能多次重新分配空间，性能较差
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(i);
    }

    // 问题点：请找出
    // 【问题点】在头部插入，引发扩容，且后续元素全部后移 1 位（复杂度为 O(n)）
    for (int i = 0; i < 100; i++) {
        vec.insert(vec.begin(), i);
    }

    // 问题点：请找出
    std::vector<int> data2;
    // 【问题点1】预留空间过少，大批量尾插时仍会触发扩容
    data2.reserve(10);

    for (int i = 0; i < 1000; i++) {
        if (data2.size() == data2.capacity()) {
            // 【问题点2】多余的手动扩容 -> 编译器会自动扩容至 1.5 - 2 倍
            data2.reserve(data2.capacity() * 2);
        }
        data2.push_back(i);
    }
}

int main() {
    vectorOperations();
    iteratorProblems();
    emplaceVsPush();
    capacityManagement();
    algorithmIssues();
    performanceTraps();

    return 0;
}
