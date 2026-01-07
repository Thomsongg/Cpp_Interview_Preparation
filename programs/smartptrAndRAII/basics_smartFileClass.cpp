// 智能指针 & RAII
#include <iostream>
#include <memory>
#include <string>
#include <cstdio>

using namespace std;

/*
 * 演示智能指针与传统指针
 */

// 传统指针：手动管理内存，申请后应及时释放
// 缺点：1. 复杂的业务逻辑下，每个分支的内存释放都要考虑到
//       2. 非异常安全：申请内存 (new/malloc) 到 释放(delete/free) 过程中抛出异常，则无法执行释放，导致内存泄漏
void processOrder_traditional()
{
    // 使用原始指针，手动申请，分配到堆内存
    int* data = new int[100];

    // 简化处理：内部定义一个业务逻辑函数
    auto checkInventory = []() -> bool {return false;};

    // 最大的缺点：繁琐！
    // 每一个可能导致异常的分支，都需要做内存释放
    if (!checkInventory())
    {
        delete[] data;
        return;
    }

    auto doComplexCalculation = [](int* input) -> void {;};

    // 需要在业务逻辑中布置 try-catch, 手动创造异常安全场景
    // 在抛出异常时，手动释放内存
    try
    {
        doComplexCalculation(data);
    }
    catch (...)
    {
        delete[] data;
        throw;
    }

    // 正常结束也需要释放
    delete[] data;
}

// 【改进】智能指针
// 以 unique_ptr 举例
void processOrder_modern()
{
    // 使用智能指针申请内存
    auto data = std::make_unique<int[]>(100);

    auto checkInventory = []() -> bool {return false;};
    if (!checkInventory())
    {
        // 可直接返回，无需单独 delete
        return;
    }

    auto doComplexCalculation = [](int* input) -> void {;};
    
    // 异常安全：无需手动 try-catch, 即使发生异常也能自动释放内存
    doComplexCalculation(data.get());
}

// 【详解】智能指针的使用
// 定义一个类
struct Player
{
    string name_;

    Player(string name) : name_(name) {}
    void play()
    {
        cout << "Player " << name_  << " is playing...\n";
    }
};

// 一个只支持原生 C 语言的函数
int copyCharArray(int* p)
{
    return *p;
}

void display_usages()
{
    // 1. 智能指针的创建 -> 推荐使用 make_unique / make_shared 代替 new
    auto unique = std::make_unique<Player>("Tommy");
    auto shared = std::make_shared<Player>("Elma");

    // 2. 调用指针对象的成员 -> 使用指针 -> 或 * 解引用
    unique->play();
    (*shared).play();

    // 3. 使用 .get() 返回原始指针，但不转让所有权
    //    适用于只接收原始指针的原生 C语言 API
    auto num_ptr = std::make_unique<int>(10);
    cout << "Call function copyCharArray: " << copyCharArray(num_ptr.get()) << "\n";

    // 4. 手动释放智能指针 -> 使用 .reset() 或 置空 nullptr
    auto num2_ptr = std::make_shared<int>(200);
    num2_ptr.reset();
    num_ptr = nullptr;
}

// 【实战】实现一个智能化“文件管理类”
//  痛点：文件句柄 (Handle) 使用完毕后自动释放，防止长期占用，阻塞其他有需要的进程/线程
class SmartFile
{
private:
    // 参数1：FILE*（指针对象）类型的智能指针
    // 参数2：Deleter 类型，调用 fclose 的函数对象，实现指针析构时关闭文件
    std::unique_ptr<FILE, decltype(&std::fclose)> file_ptr;

public:
    // 构造函数：RAII 式获取资源
    // 使用原生 C 字符串(char*) 和 fopen, 尝试打开文件
    SmartFile(const char* filename) : file_ptr(nullptr, &std::fclose)
    {
        FILE* raw_ptr = nullptr;
        errno_t err = fopen_s(&raw_ptr, filename, "w");

        if (err != 0 || raw_ptr == nullptr)
        {
            cout << "无法打开文件，错误代码: " << err << " \n";
        }
        else
        {
            // 将 FILE* 原始指针托管给 file_ptr
            file_ptr.reset(raw_ptr);
            cout << "文件已安全打开。\n";
        }
    }

    // 写文件操作
    // 使用 string 类字符串，转换为原生 C 字符串后，使用 fputs 写入文件
    void write(const string& text)
    {
        if (file_ptr)
        {
            std::fputs(text.c_str(), file_ptr.get());
            cout << "成功写入数据: " << text << "...\n";
        }
        else
        {
            cout << "无法写入文件，请重试!\n";
        }
    }

    // 无需自定义析构函数，手动调用 fclose
    // 作用域结束时，自动调用 file_ptr 的删除器，调用 fclose
};

void demo()
{
    cout << "------测试 SmartFile 智能文件管理类的功能------\n";

    // 初始化文件管理对象
    SmartFile myFile("test.txt");
    myFile.write("Hello RAII!");

    if (true)
    {
        cout << "遇到某种情况，提前返回...\n";
        return;
    }
}


int main()
{
    //display_usages();
    demo();

    std::cout << "-----函数已结束-----\n";
}
