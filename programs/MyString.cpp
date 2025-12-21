#include<iostream>
#include<stdexcept>
#include<memory>
#include<utility>

// 用原始指针实现 String 类
class MyString
{
private:
    char* m_data;
    size_t m_size;

    // 辅助函数：分配内存并复制字符串
    void copy_string(const char* str, size_t len)
    {
        // 分配时，多预留 1 byte 用于结束符
        m_size = len;
        m_data = new char [len + 1];
        std::memcpy(m_data. str, m_size);
        m_data[m_size] = '\0';
    }

    // 辅助函数：清空字符串并释放内存
    void cleanUp() noexcept
    {
        if (m_data)
        {
            // 销毁后指针置 null
            delete[] m_data;
            m_data = nullptr;
        }
        m_size = 0;
    }

public:
    // 构造函数：默认、参数、析构、拷贝、移动
    MyString() : m_data(nullptr), m_size(0) {}

    // 参数构造：外部调用，从 C 字符串构造
    explicit MyString(const char* str)
    {
        m_data = nullptr;
        m_size = 0;

        if (str)
        {
            copy_string(str);
            m_size = std::strlen(str);
        }
    }

    // 拷贝构造：通过传入的对象，进行复制
    MyString(MyString& other)
    {
        m_data = nullptr;
        m_size = 0;

        if (other.m_data)
        {
            copy_string(other.m_data);
            m_size = other.m_size;
        }
    }

    // 重载拷贝构造运算符：注意 this 指针的判断，并返回当前实例 *this
    MyString& operator = (MyString& other)
    {
        // 需要使用 this 指针，判断当前示例与入参是否重合
        if (this != other)
        {
            // 先清理本对象，再复制
            cleanUp();
            copy_string(other.m_data);
            m_size = other.m_size;
        }

        // 返回当前对象实例
        return *this;
    }

    // 移动构造：通过 move 转移所有权（自动将原对象置空），且不抛出异常
    MyString(MyString&& other) noexcept
    {
        m_data = std::move(other.m_data);
        m_size = other.m_size;

        other.m_size = 0;
    }

    // 重载移动构造运算符
    MyString& operator = (MyString&& other) noexcept
    {
        if (this != other)
        {
            m_data = std::move(other.m_data);
            m_size = other.m_size;

            other.m_size = 0;
        }

        return *this;
    }

    // 析构函数：清理并释放内存
    ~MyString()
    {
        cleanUp();
    }

    // 通用方法：返回 C 风格字符串
    const char* get_c_str() const noexcept
    {
        return m_data ? m_data : "";
    }

    // 通用方法：返回字符串有效长度
    const size_t get_size() const noexcept
    {
        return m_size;
    }
}

