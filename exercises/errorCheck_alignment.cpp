#include <iostream>
#include <memory>   // 智能指针
#include <vector>
#include <stdexcept>

// 【问题点】内存未对齐
struct BadAlignment {
    char a;     // 1
    double b;   // 8
    int c;      // 4
    char d;     // 1
};
// 占用 1 + (7) + 8 + 4 + 1 + (3) = 24

// 【修改】优化后的结构体
struct GoodAlignment
{
    char a;
    char d;
    int c;
    double b;
};
// 占用 1 + 1 + (2) + 4 + 8 = 16

class ProblematicClass {
    // 【问题】flag 和 data[] 内存未对齐
    /*char flag;
    int* data;
    double value;
    short count;*/

    // 【修改】修改顺序，flag(char) 和 count(short) 放在一起
    char flag;      // 1 byte
    short count;    // 2 bytes
    int* data;      // 4 * n bytes (n = 2 的倍数时，data 和 value 可以保证内存对齐)
    double value;   // 8 bytes

public:
    ProblematicClass() : data(nullptr), value(0.0), count(0) {}

    // 【问题】资源所有权不明确 -> 潜在的内存泄漏
    //  问题的根源：结构体成员的内存申请，与构造函数分离
    void setData(int size) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
    }

    // 【问题点】数组类型的内存销毁
    //~ProblematicClass() {
    //    delete data;  // 问题点1
    //}
    // 【修改】按数组长度销毁，且保证异常安全
    ~ProblematicClass() {
        try
        {
            delete[] data;
        }
        catch (...)
        {
            // 模拟异常日志记录
            std::cout << "[ProblematicClass] delete error!";
        }
    }
};

// 下面使用多种方法，解决结构体成员 int* data 多重析构导致程序崩溃的问题
// 示例1：智能指针法
class AutomaticClass
{
    char flag;      // 1 byte
    short count;    // 2 bytes
    std::unique_ptr<int[]> data;      // 4 * n bytes
    double value;   // 8 bytes
    size_t size;    // 建议补充：内存大小

public:
    // 默认构造
    AutomaticClass() : flag(0), count(0), value(0.0), size(0) {}

    // 建议操作：setter 和 构造分离
    // 使用参数构造创建对象
    AutomaticClass(size_t newSize) : flag(0), count(0), value(0.0), size(newSize)
    {
        std::cout << "Call AutomaticClass::AutomaticClass(Param), newSize: " << newSize << "\n";
        data = std::make_unique<int[]>(newSize);
    }

    // 支持移动构造
    AutomaticClass(AutomaticClass&& other) noexcept
        : flag(other.flag), count(other.count), value(other.value), size(other.size)
    {
        std::cout << "Call AutomaticClass::AutomaticClass(Move)\n";
        data = std::move(other.data);
        other.data = nullptr;
    }

    // 支持移动运算符
    AutomaticClass& operator=(AutomaticClass&& other) noexcept
    {
        if (this != &other)
        {
            flag = other.flag;
            count = other.count;
            size = other.size;
            value = other.value;
            data = std::move(other.data);
            other.data = nullptr;
        }
        return *this;
    }

    // setter: 通过 size 设置，这里可不填参数
    void setData()
    {
        if (size == 0)
        {
            return;
        }

        for (int i = 0; i < size; i++)
        {
            data[i] = i;
        }
        std::cout << "AutomaticClass::setData calling success!\n";
    }

    // 不需要单独修改析构函数，unique_ptr 会自动析构
};

int main() {
    std::cout << "Size of BadAlignment: " << sizeof(BadAlignment) << std::endl;

    // 【调试】打印内存对齐后的结构体
    std::cout << "Size of GoodAlignment: " << sizeof(GoodAlignment) << std::endl;

    // 创建对象数组
    ProblematicClass* arr = new ProblematicClass[5];

    // 【问题点】资源所有权不明确
    // 为每个对象设置数据
    for (int i = 0; i < 5; i++) {
        arr[i].setData(10);  // 问题点2
    }

    // 这里如果使用浅拷贝创建对象，在离开作用域时会导致“双重释放”
    //ProblematicClass newObj = arr[0];

    // 离开作用域时：
    // (1) newObj 先析构 -> delete[] data
    // (2) arr[0] 再析构 -> delete[] data, 尝试释放相同内存，data 指针悬挂，导致“双重释放”，运行时报错

    // 【修改】使用智能指针管理 data, 在新的类 AutomaticClass 中实现 （在作用域最下方演示）

    // 【问题点】对象数组销毁，应使用 delete[]; delete arr 只会销毁第一个对象，导致内存泄漏
    //delete arr;  // 问题点3
    delete[] arr;

    // 结构体数组，可能有对齐问题
    BadAlignment alignArray[10];
    char* ptr = reinterpret_cast<char*>(alignArray);

    // 尝试访问可能未对齐的地址
    double* dPtr = reinterpret_cast<double*>(ptr + 1);  // 问题点4
    *dPtr = 3.14;

    // 【问题】ptr 已将结构体对象数组 alignArray[10] 转换为 char*
    //         dPtr 又按 double* 访问, 执行 dPtr = ptr + 1 后, dPtr 指向未对齐的地址，即第 8 个字节 (alignArray[0].a 的后一个 byte)

    // 【修改】使用优化后的结构体，进行访问和赋值；
    // 一般不建议对内存未对齐的结构体指针做类型转换
    GoodAlignment newAlignArray[10];
    newAlignArray[0].b = 3.14;
    std::cout << "Value of b in newAlignArray[0]: " << newAlignArray[0].b << "\n";

    // 使用智能指针类的实现，完美规避“多次析构”的问题
    
    // 创建对象数组，使用 vector 创建
    std::vector<AutomaticClass> vec_newArr;
    vec_newArr.reserve(5);

    // 为每个对象设置数据
    for (int i = 0; i < vec_newArr.capacity(); i++)
    {
        // 使用 emplace_back 直接构造 （推荐）
        //vec_newArr.emplace_back(10);

        // 或参数构造临时对象后，使用通过 move 传入，使用移动构造
        AutomaticClass newClass(10);
        vec_newArr.emplace_back(std::move(newClass));

        vec_newArr[i].setData();
    }

    return 0;
}
