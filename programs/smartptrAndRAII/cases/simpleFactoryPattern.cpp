// 智能指针 & RAII
#include <iostream>
#include <memory>
#include <string>

using namespace std;

/*
 * 示例1: 使用 unique_ptr 实现工厂模式 (Factory Pattern)
 * 工厂模式简介：
 *      1. 由统一的工厂类，生产不同类型的对象（派生类）
 *      2. 返回泛类（基类）的指针 -> 如生产圆形、矩形等，返回“形状”类指针
 *      3. 生产后，通过 unique_ptr 移交对象所有权
 */

// 基类：形状
// 多态性：通过虚函数、虚析构实现
class Shape
{
public:
    // 虚函数 draw(): 由派生类重写
    virtual void draw() = 0;

    // 【注意】基类必须添加虚析构函数
    virtual ~Shape()
    {
        cout << "Shape has been destroyed!\n";
    }
};

// 派生类1：圆形
class Circle : public Shape
{
public:
    // 重写基类的 draw() 虚函数
    void draw() override
    {
        cout << "Drawing a circle.\n";
    }

    // 派生类的析构函数
    ~Circle()
    {
        cout << "Circle has been destroyed!\n";
    }
};

// 派生类2：矩形
class Rectangle : public Shape
{
public:
    void draw() override
    {
        cout << "Drawing a rectangle.\n";
    }

    ~Rectangle()
    {
        cout << "Rectangle has been destroyed!\n";
    }
};

// 工厂类：专门生产 Shape 对象并返回 Shape*
class Factory
{
public:
    // 函数：使用 unique_ptr 创建 Shape 对象，返回对应的 Shape*
    // 声明 static: 创建的 Shape 类智能指针，需要提供给类外，是类的公共方法
    static std::unique_ptr<Shape> createShape(const string& type)
    {
        if (type == "Circle")
        {
            // 返回的指针，使用 make_unique 安全创建
            return std::make_unique<Circle>();
        }
        else if (type == "Rectangle")
        {
            return std::make_unique<Rectangle>();
        }

        return nullptr;
    }
};

int main()
{
    // 使用工厂类的生成器（createShape），创建一个形状
    // 返回的智能指针，作用域仅限 main 中，结束后自动析构
    std::unique_ptr<Shape> myShape = Factory::createShape("Circle");
    if (myShape != nullptr)
    {
        myShape->draw();
    }

    std::cout << "-----函数已结束-----\n";

    // 结束后对象自动析构，调用派生类和基类的析构函数
}
