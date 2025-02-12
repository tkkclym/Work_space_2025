Constructors,Destructors,and Assignment Operators

# 构造，析构和赋值运算

## 条款5：了解C++默默编写并调用哪些函数

​	对于创建类的时候自己未声明的一些函数，编译器会为其声明以下几种函数:default构造函数，default析构函数，copy函数以及拷贝赋值运算符（copy assignment 运算符）。

拷贝构造函数没有返回值，拷贝赋值运算符重载需要返回值，因为一个是构造行为，一个是重载了运算符

```c++
#include <iostream>
using namespace std;
class Base{
public：
    Base(){ cout<<"Base Default Destructor"<<endl;}
    Base(const Base& base){
	cout<<"Base Default Destructor"<<endl;
    }
};

class Derived:public Base{
private:
    int member;
public:
    Derived():member(0){cout<<"Base Default Destructor"<<endl; }
    Derived(const Derived& other):Base(other),member(other.member){
	cout<<"Derived copy constructor"<<endl;
    }
};

```



分不清为什么嘛会有`拷贝构造函数`和`拷贝赋值运算符`吗？

时候用的时候很好区分：

```c++
int main(){
    Derived d1;
    Derived d2(d1);//这就是拷贝函数的使用
    Derived d3;
    d3 = d1; //这就是拷贝复制运算符的使用 等号就是拷贝赋值运算符
    
}
```

## 条款12：赋值对象的时候不要忘记其每一个成分

​	设计良好的面向对象系统会将对象内部封装起来，只留两个函数负责对象的拷贝（复制），那便是带着适切名称的copy构造函数和copy assignment 运算符。**（我们称他们为copying函数）**

## 重点：

​		任何时候，只要你承担起了为derived class撰写copying函数的重任，必须很小心的为其复制其Base class的部分，而那些成员变量通常是private 的，无法直接访问，所以应该让derived class copying构造函数**调用**相关的Base class 的**copy构造函数**。

代码解释：

```c++

```







### Question:

如果派生类对象有自己的成员变量，如果不写拷贝构造函数会有什么后果？编译器默认的拷贝构造函数对派生类对象有什么处理吗？

当派生类有自己的成员变量，且不编写拷贝构造函数时，编译器会为派生类生成一个默认的拷贝构造函数。默认拷贝执行的是浅拷贝，执行的只是将`一个对象`的`成员变量`复制到`另一个对象`的`对应的成员变量`中

如果对象成员变量中`包含指针`并且`指针动态分配内存`，那么浅拷贝会导致两个对象的成员指针指向同一块内存,会导致以下两个问题：

**内存泄漏：**当一个对象被销毁的时候，会释放他所指向的内存。另一个指针仍指向这块已经被释放的内存，会成为野指针，后续再调用这个野指针会导致未定义行为。

**数据不一致：**如果一个指针修改了所指向内存的数据，那么，另一个对象也会受影响，因为他们指向同一块内存，所以还是深拷贝，对拷贝的对象单独分配内存空间。

### 防止出现浅拷贝带来的问题：

所以还是要自己写拷贝构造函数。

为解决默认拷贝构造函数中数据浅拷贝带来的问题，在写构造函数的时候，对于成员函数有指针数据类型的，要在拷贝构造函数中动态分配内存空间。

注意对于类中成员变量包含指针的，在析构的时候记得delete指针成员变量。

