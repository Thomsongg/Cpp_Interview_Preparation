# C++ & CUDA开发 八股专题

## 1 C++基础

### 1.1 C++的构造函数类型

##### 默认构造函数
无参数/所有参数都有默认值，编译器自动生成
```cpp
class MyClass
{
  private:
      int data;
  public:
      MyClass()
      {
          data = 1;
      }
}
// 使用
MyClass myClass;
```

##### 参数构造函数
有参数的初始化，不带explicit会隐式类型转换
```cpp
class MyClass
{
  private:
      int data;
  public:
      // 参数构造函数，可使用移动语义为成员赋值
      MyClass(int value) : data(value){}
}
// 使用
MyClass myClass(10);
```

##### 拷贝构造函数
创建一个已有对象的副本(深拷贝); 可以重载‘=’运算符；参数必须是引用传递
```cpp
class MyClass
{
  private:
      int data;
  public:
      // 参数构造函数，可使用移动语义为成员赋值
      MyClass(MyClass& other) : data(other.data){}
}
// 使用
MyClass myClass1(10); // myClass1使用参数构造
MyClass myClass2 = myClass1;  // 使用'=' 调用拷贝构造函数；隐式的类型转换(不重载运算符时)
```

##### 移动构造函数(C++11)
创建一个对象的浅拷贝，可视作其引用，直接使用原对象的内存空间；使用关键字 noexcept
```cpp
class MyClass
{
  private:
      float[] data;
      int size;
  public:
      // 参数构造函数，可使用移动语义为成员赋值
      MyClass(MyClass&& other) noexcept : data(other.data), size(other.size)
      {
          // 将原对象置于有效且可析构的状态
          other = nullptr;
          other.data = 0;
      }
}
// 使用
MyClass myClass1(10);
MyClass myClass3 = std::move(myClass1);  // 转移原对象的控制权，显式移动
```

##### 委托构造函数(C++11)
当构造的逻辑与其他构造函数存在重复时，委托其他构造函数，一般用于参数构造函数的重载；节省重复代码
```cpp
class MyClass
{
  private:
      std::string name;
      int age;
      std::string address;
  public:
      // 主要的构造函数：参数构造函数
      MyClass(const std::string& n, int a, const std::string& addr) : name(n), age(a), address(addr){}

      // 委托构造函数：使用部分参数初始化，故直接委托主构造函数(直接传参)
      MyClass(const std::string& n) : MyClass(n, 0, ""){n, 0, "none"}

      // 默认构造函数也可委托
      MyClass() : MyClass("unknown", 0, "none")
}
// 使用
MyClass myClass1;   // 默认构造函数，委托主构造函数进行初始化
MyClass myClass2 = MyClass("Tom");  // 委托构造函数，传一个参数，其他参数默认
MyClass myClass3 = MyClass("Jerry", 12, "House");   // 正常的参数构造
```

##### 继承构造函数(C++11)
用于子类(派生类)；子类直接继承父类的构造函数；不影响子类独有的构造函数
```cpp
class Son : public MyClass
{
  public:
      // 继承父类的所有构造函数
      using MyClass :: MyClass;

      // 增加子类独有的构造函数，这里使用了委托
      Son(const std::string& n) : MyClass() {}
}
```

### 1.2 拷贝与引用机制

#### 1.2.1 深浅拷贝

#### 1.2.2 左值与右值

#### 1.2.3 移动语义

#### 1.2.4 完美转发

### 1.3 泛型编程 (模板元编程)
通用型模板、策略模式、类型特化、编译时计算、类型安全
**优点：**
1. 零成本抽象：编译时计算无运行时开销
2. 类型安全：编译时捕获类型错误
3. 性能优化：生成针对特定类型的最优代码
4. 代码生成：根据类型特性自动生成特化代码
5. 设计灵活性：支持策略模式、特性基编程等高级设计模式

**注意：**
- 编译时间可能显著增加
- 错误信息可能难以理解
- 调试困难
- 需要平衡编译时和运行时的复杂度

#### 1.3.1 常用的模板编程 【基础】

#### 1.3.2 模板元编程 【进阶】

##### 1.3.2.1 编译时计算

###### 基础编译时计算

###### 编译时条件判断

##### 1.3.2.2 类型安全

###### 类型特性 (Type Traites)

###### 替换失败非错误 SFINAE (Substitution Failure Is Not An Error)

##### 1.3.2.3 编译时数据结构

##### 1.3.2.4 编译时算法

##### 1.3.2.5 表达式模板 【高级】

##### 1.3.2.6 概念 (Concepts, C++20) 【高级】

##### 1.3.2.7 在CUDA中的高级应用

## 2 高性能 CUDA C++ 开发
### 2.1 高级内存管理 & 系统编程
#### 2.1.1 【重要】智能指针 (C++11)
智能指针旨在解决原生指针管理内存时，可能出现的 new和delete不同步，引发的内存泄漏问题。
我们看下面的例子，构造函数抛出异常时，部分成员已创建，引发内存泄漏： 
```cpp
class DangerousClass
{
  private:
      char* data;
      int length;
  public:
      // 构造函数
      DangerousClass(const char* str, int length)
      {
          data = new int[length];  // 申请内存
          strcpy(data, str);    // 注意：此处如果抛出异常，之前的内存申请将会引发内存泄漏！！！
      }
}
```
**解决方法：** 使用智能指针处理，在变量生命周期结束时自动释放内存。

##### unique_ptr

##### shared_ptr & weak_ptr

#### 2.1.2 移动语义 (C++17)

#### 2.1.3 RAII 思想

#### 2.1.4 泛型编程 (模板元编程)

### 2.2 高并发低延时 & 线程并行

#### 2.2.1 C++异步编程 (C++11)

#### 2.2.2 原子操作 (C++11)

#### 2.2.3 无锁数据结构

#### 2.2.4 协程 (C++20)

#### 2.2.5 线程并行算法 (C++17)

### 2.3 高性能编译
#### 2.3.1 编译器优化

#### 2.3.2 内联汇编

