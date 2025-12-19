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

| 构造函数类型  |  特点 | 使用场景  |
| :------------: | :------------: | :------------: |
| 默认构造函数  | 无参数，自动生成  | 数组创建，容器元素  |
| 参数化构造函数  |  有参数，可重载 |  特定初始状态 |
| 拷贝构造函数  | 深拷贝/浅拷贝  |  对象复制，值传递 |
| 移动构造函数  |  资源转移，高性能 |  临时对象，返回值 |
| 委托构造函数  |  构造函数复用 | 减少代码重复  |
| 继承构造函数  |  基类构造函数继承 | 派生类构造简化  |

### 2. 智能指针
1. shared_ptr
2. weak_ptr
3. unique_ptr

#### (1) shared_ptr
**核心：** 通过引用计数，允许多个指针共享对同一个对象的所有权

**原理：** 由两个**控制块**组成。当创建一个 shared_ptr 对象时，其内存中的结构如下：
1. 原始指针：指向实际管理的资源
2. 控制块指针：指向辅助的数据结构
    - 强引用计数：记录有多少个 shared_ptr 指向此对象
    - 弱引用计数：记录有多少个 weak_ptr 指向此对象
    - 删除器：自定义的析构函数
    - 分配器：自定义的内存分配方式

**内部方法：**
- `use_count()` 获取引用计数
- `reset()` 重置指针，将原始指针释放并销毁 (其他引用此对象的指针计数 -1)
- `get()` 获取原始指针
- `swap()` 交换两个指针所拥有的 shared_ptr 对象
- `shared_from_this()` 返回使用对象自身的 shared_ptr

**引用计数：** shared_ptr 控制块中的强引用计数器，控制 shared_ptr 是否析构。
- 每当有其他指针对当前 shared_ptr 进行引用（复制操作 `std::shared_ptr<int> p2 = p1`）时，引用计数 +1
- 当一个复制的 shared_ptr 离开作用域时，引用计数 -1
- 当引用计数为 0 时，进行析构

**用法：** 
1. 初始化 `std::shared_ptr<MyClass> ptr1 = std::make_shared<MyClass>()`
2. 指针复制后引用计数的变化

```cpp
// shared_ptr 的用法
void useShared()
{
    // 创建 shared_ptr
    auto sp1 = std::make_shared<int>(100);
    // 输出 sp1 的引用计数，此时为 1
    std::cout << "Count: " << sp1.use_count() << "\n";

    // 另一个指针引用，在单独的作用域
    {
        // 另一个指针指向 sp1, sp1 的引用计数 为 2 (原基础上 +1)
        std::shared_ptr<int> sp2 = sp1;
        std::cout << "Now count changes to :" << sp1.use_count() << "\n";
    }

    // sp2 的作用域结束，sp1 引用计数回到 1
    std::cout << "Count: " << sp1.use_count() << "\n";
}
```

**shared_ptr 的线程安全：**
- 引用计数是**原子操作**，所有其加减是线程安全的
- 修改 shared_ptr 指针指向时，可能不安全。同一个 shared_ptr 被多个线程（或指针）读（即复制）是安全的；被多个线程**写**是不安全的。

```cpp
void fnc(shared_ptr<A>& sp)
{
    // ...
    if (...)
    {
        // sp 被其他指针写入，计数 -1
        sp = other_sp1;
    }
    else if (...)
    {
        // sp 被其他指针写入，计数 -1
        sp = other_sp1;
    }
}
```
如上述示例，当若干线程都在修改 sp 的指向时，回导致某线程使用时，sp 的计数已被修改（甚至为 0），导致此时 sp 被提前析构，报错 core dump

解决方法：给线程加锁
- 使用线程锁 `std::mutex`
- 共享指针锁 `std::shared_mutex`

```cpp
#include<memory>
#include<mutex>

// 公共 shared_ptr
std::shared_ptr<const Data> data_;

// 线程锁
mutable std::mutex mutex_;

// 【可选】shared_mutex 共享指针锁
mutable std::shared_mutex shared_mutex_;

// 读操作：无锁
int getVal() const
{
    auto data = data_; // 原子拷贝
    return data ? data->value : ;
}

// 写操作：加锁
void setValue(int new_value)
{
    // 对线程加锁
    std::lock_guard<std::mutex> lock(mutex_);

    // 或使用共享指针锁
    std::shared_lock<std::shared_mutex> lock(shared_mutex_);

    // 拷贝 (shared_ptr 非空则拷贝)
    auto new_data = data_ ? std::make_shared<Data>(*data_) : std::make_shared<Data>();

    new_data->value = new_value;

    // 原子替换（移动语义实现）
    data_ = std::move(new_data);
}
```

#### (2) weak_ptr
**核心：** 解决 shared_ptr 可能存在的**循环引用**的问题。

**原理：** weak_ptr 被 shared_ptr 引用时，不会增加对方的引用计数。这样可以打破 shared_ptr 的循环引用，使作用域结束时引用计数正常-1，可以被析构，避免内存泄漏。
- shared_ptr 对象创建 weak_ptr 对象时，计算器没有变化
- weak_ptr 的构造和析构，也不影响引用计数
- weak_ptr 没有重载 解引用运算符 `*`, 不操作资源

```cpp
struct A
{
    shared_ptr<B> b;
};

struct B
{
    weak_ptr<A> a;
};

// 创建两个 shared_ptr, 模拟循环引用
shared_ptr<A> pa = make_shared<A>();
shared_ptr<B> pb = make_shared<B>();

// 循环引用：让 pa 和 pb 互相持有对方的 shared_ptr
// pa->b = pb, 即将 shared_ptr 赋给 weak_ptr, 不增加 shared_ptr 的引用计数
pa->b = pb;
pb->a = pa;
```

**内部方法：**
- `expired()` 判断所指向的原指针是否被释放，返回 true / false
- `use_count()` 返回原 shared_ptr 的引用计数
- `lock()` 返回原 shared_ptr, 为空则返回空的 shared_ptr
- `reset()` 将自身置空

### 3. 面向对象 (OOP)
#### 封装
将抽象得到的数据和行为有机结合，通过类把数据和函数合并为成员，将对象的使用者和设计者分开，隐藏实现细节（如私有成员），增加代码安全系数，提高可维护性和可拓展性。

#### 继承
即派生，是从已有类到新类的过程。子类继承基类后，可以通过对象调用基类的成员或方法。

**继承方式：**
- 单一继承：子类只继承一个父类
- 多重继承：一个子类继承多个父类，父类之间用逗号隔开，父类名之前要有继承权限；如果多个父类有公共的变量或方法，子类调用时要加限定符 `obj.classA::i = 1`

**继承权限：** 子类中访问的权限，根据以下两种的最低权限为准
1. 父类中访问权限 (public/protected/private)
2. 继承方式 (public/protected/private)

父类访问权限为 private, 子类一定没有访问权限；继承方式如果为 private, 不论父类访问权限是什么，子类访问权限一定不高于 private

#### 多态
即使用同一个接口，而通过不同的类/函数实现。
- 静态多态：不同类的实现中要求相同的接口声明；即模板的具现化
- 动态多态：对于有公共父类的几个类，确定一个共同功能集，在父类中对各个类声明对应的虚函数接口；在子类重写这些虚函数，作为特定的实现

C++的多态：
- 重载：函数重载/运算符重载，编译期
- 虚函数：子类的多态性，运行期
- 模板：类模板、函数模板，编译期

#### 虚函数
**作用：**
- 允许在子类中重写父类的函数
- 实现运行时多态（动态绑定）
- 通过父类指针 / 引用，调用实际对象的函数

**机制：**
1. 虚函数表 (vtable)
    - 每个包含虚函数的类（新创建的虚函数 或 重写父类的虚函数），有一个**独有的虚表**
    - 虚表是**编译时生成**的**静态数据**
    - 存储该类所有虚函数的地址（**函数指针数组**）
    
2. 虚函数指针 (vptr)
    - 每个包含虚函数的对象，都有一个隐藏的虚函数指针
    - bptr 指向该对象**所属类的虚表**
    - vptr 通常位于对象**内存布局的开头**

**虚函数的调用过程：**
1. 通过对象的 vptr 找到类的虚函数表 (第一次寻址)
2. 在虚表中找到虚函数 func1 对应的索引 (第二次寻址)
3. 通过函数指针调用实际函数

**虚函数的性能开销：**
- 每次调用时需要**两次间接寻址**
- 对象需要额外的空间存储虚函数指针 vptr; 多重继承下 vptr 占用空间 = 4bytes * N (N 为包含虚函数的父类个数)
- 虚表 vtable 会占用大量空间，即父类虚函数 + 子类虚函数指针的体积

继承关系存在时，子类的虚表除重写的虚函数接口外，其余部分与父类的虚表重复。菱形继承会产生极大开销：如果存在大量的子类继承，且重写父类的虚函数接口只占总数的一小部分的情况下，会造成大量地址空间浪费

**注意事项：**
- **构造函数不能为虚函数**。
- 虚函数的父类，必须声明为**虚析构函数**。

说明：
1. 虚函数表需要在对象实例化后才能被调用。对象构造时，尚未给虚表分配内存，违反了“先实例化再调用”的原则。
2. 如果父类的析构函数不是虚函数，则析构时 (使用 delete) 只会调用父类的析构，而非实例的类型。
3. 不能是虚函数的函数：
    - 友元函数：不是类成员
    - 静态成员函数：static 成员不属于任何实例；且static 成员函数没有 this 指针，找不到虚函数
    - 内联函数
    - 构造函数
    - 成员函数模板

```cpp
// 父类 Employee 的析构不是虚函数
// 析构时，调用的是 Employee 类 (即指针类型的析构)，而非 Singer 类的析构函数
Employee* pe = new Singer;
delete pe;
```

#### 面向对象和面向过程
面向过程：根据解决问题的步骤，用函数依次实现，依次调用函数
面向对象：把问题分解为不同的对象，用来描述对象在整个解决问题的步骤中的行为

面向过程的特点：
- 优点：性能较高，避免了类实例化为对象的开销
- 缺点：可维护性差、不易复用

面向对象的特点：
- 优点：已维护、易复用和拓展，可以通过封装、继承和多态，设计出低耦合的系统，提升灵活性
- 缺点：性能较差

### 4. 内存对齐
#### 内存对齐的规则 (操作系统)

#### 结构体 (struct) 和 共同体 (union) 的区别


## 二、Effective Modern C++
### 1 类型推导
考虑一个函数模板，函数入参可能有三种类型：
(1) 入参是一个指针或引用，非通用引用

```cpp
template<typename ParamType>
void f(ParamType& param)     // 指针或引用
```
此时实参的引用会被忽略

(2) 入参是一个通用引用
```cpp
template<typename ParamType>
void f(ParamType&& param)     // 通用引用

int x=27;                       //如之前一样
const int cx=x;                 //如之前一样
const int & rx=cx;              //如之前一样

f(x);                           //x是左值，所以T是int&，
                                //param类型也是int&

f(cx);                          //cx是左值，所以T是const int&，
                                //param类型也是const int&

f(rx);                          //rx是左值，所以T是const int&，
                                //param类型也是const int&

f(27);                          //27是右值，所以T是int，
                                //param类型就是int&&
```
对于通用引用，实参param类型会根据左值或右值区分：
- 入参是左值：保留const和引用，如 const ParamType&
- 入参是右值：抹除const，作为右值引用ParamType&&

(3) 入参非指针和引用
```cpp
template<typename ParamType>
void f(ParamType param)     // 直接传参
```
此时实参是一个拷贝，原本修饰的const 或 volatile会被抹除。

模板类型推导时，根据以上场景存在如下特性：
- 在模板类型推导时，有引用的实参会被视为无引用，他们的引用会被忽略
- 对于通用引用的推导，左值实参会被特殊对待
- 对于传值类型推导，const和/或volatile实参会被认为是non-const的和non-volatile的
- 在模板类型推导时，数组名或者函数名实参会退化为指针，除非它们被用于初始化引用

### 2 auto

### 3 现代C++
#### 3.1 括号()与花括号{}初始化

#### 3.2 nullptr与NULL

#### 3.3 别名(using)和typedef

#### 3.4 限域枚举 (enum class)

### 4 智能指针

### 5 右值引用、移动语义、完美转发

### 6 Lambda表达式

