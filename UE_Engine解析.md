# UE 智能指针



> 这里智能指针是UE自己实现的



注意智能指针只能定义自定义类使用，U类不能使用智能指针，意思就是只要是继承自UObject的对象不能创建智能指针？

#### 首先说说常规共享指针：

TSharedPtr,TWeakPtr这类基于引用计数的，不适用于UObject派生类，因为UObject有自身的垃圾回收机制，和基于引用计数的智能指针管理方式不同。要是使用TSharedPtr和TWeakPtr管理UObject对象，可能会引发生命周期混乱，造成内存泄露或者悬空指针的问题。



#### 虚幻引擎为UObject提供智能指针：

​	TWeakObjectPtr和SoftObjectPtr，他们可以安全的管理UObject派生类对象。

**TWeakObjectPtr：**

​	也是弱引用指针，不会阻止UObject派生类对象被垃圾回收，当UObject被销毁的时候，TWeakObjectPtr会自动变无效，这样能避免悬空指针。

**TSoftObjectPtr：**

​	用于软引用，允许在运行时延迟加载对象，他存储的时对象的引用，非对象本身，只有在需要时才会加载对象，节省空间。







#### 智能指针使用注意事项：

- 智能指针重载++和--运算
- 使用智能指针的时候，不要使用普通指针变量来初始化智能指针，因为普通指针申请并用酱油内存需要程序员维护，我都使用智能指针了，你再用普通指针来设置智能指针不是本末倒置嘛。
-  智能指针通过Get()方法获取其控制的原始指针。
- 除了TSharedRef之外，其他所有类型的指针都可以通过Reset()接口来重置指针，原来的内存不一定会析构，需要注意的是，重置**共享指针**会影响所有指向它的弱指针
- 当可以选择TSharedRef和TSharedPtr的时候，优先选择TSharedRef

#### 那和C++智能指针比有什么不同呢？

- **线程安全**为可选项，更灵活

- C++一些通用接口没有实现，比如use_count(),unique()等

- 虚幻引擎的指针不允许抛出异常

- 有一个不允许为空的指针类型TSharedRef，创建或者赋值的时如果引用为空对象会触发断言。值得一提的是，TSharedRef作为引用，初始化之后可以重新分配给另一个对象。

- ```c++
  TShredPtr<MyClass> obj=makeSharable(new MyClass);
  TShredRef<MyClass> oobj=obj.ToSharedRef();
  ```



那为什么C++已经有智能指针了，而且虚幻引擎的指针还不能用于UObject系统，那么为什么UE还要实现一套自己的智能指针呢？原因有以下几个方面：

1. 保证不同编译平台或不同编译器的一致性，Std::shared_ptr不是所有平台都支持的
2. 为了能够和虚幻引擎其他容器和类型无缝衔接
3. 更好的控制平台特性，比如多线程的处理和优化，同时提供可选的线程安全功能
4. 虚幻引擎不希望有异常抛出
5. 更有利于调试。虚幻引擎自身有一套完整的调试工具和生态系统。
6. 倾向于在不需要的时候，不引用第三方库。引入第三方库会增加项目的依赖，可能导致不同版本的库之间存在兼容性问题，或者在不同的平台上需要额外的配置和支持。



## 共享指针

#### 特点：

- 沿袭所有普通智能指针的优点，能够避免内存泄漏，悬垂指针，避免指针指向未初始化的内存
- 共享所有权。支持多个共享指针，确保他们引用的数据对象永远不会失效，前提是至少有一个能在指向数据对象【和普通智能指针一样啊】
- **自动失效。**可以安全引用易变对象，无需担心出现悬挂指针
- 弱引用可以中断循环引用【和普通智能指针一样啊】
- **可选的线程安全** 
- 语法强健
- 非侵入式，但是反射依旧可以实现
- 轻量内存占用



​		这里引入了**共享引用**的概念，共享指针和共享引用的区别就是共享引用不能为空，其始终引用有效对象，在共享指针和共享引用之间选择的时候，**共享引用是首选，除非你需要一个可以为空或者空的对象**



#### 共享指针声明和初始化



```c++

创建空共享指针
TsharedPtr<FMyObjectType> EmptyPointer;
--------------
为新对象创建一个共享指针 ,此时该对象有一个引用，当引用数量为0时，该对象销毁
TsharedPtr<FMyObjectType> NewPointer(new FMyObjectType());

//if i have a objecttype AA  NewPointer是一个关键字啊
TsharedPtr<AA>=NewPointer(new AA());
---------------
从共享引用创建共享指针
TsharedRef<FMyObjectType> NewReference(new FMyObjectType());
TsharedPtr<FMyObjectType>PointerFromReferece=NewReference;
--------------
    
创建线程安全的共享指针
    
TsharedPtr<FMyObjectType,ESPMode::TharedSafe> NewZThreadSafePointer=MakeShared<FMyObjectType,ESPMode::ThreadSafe>(MyArgs);
```



和普通智能指针一致，如果复制共享指针，系统将向他引用的对象添加一个引用，对象持续存在，**直到不再有共享指针或者共享引用引用它为止**

​		那一个对象同时有一个共享引用和一个共享指针，他的引用个数是2嘛？__是的1

- Reset函数： PtrOne.reset(); PtrTwo=nullptr 一个效果，现在都是置空

- MoveTemp函数或者MoveTempIfPossible将一个共享指针内容转移至另一个共享指针，原共享指针置空，【Ue的移动语义】



#### 共享指针和共享引用相互转换

​	二者进行转换是常见的操作。共享引用隐式的转换为共享指针，并提供新的共享指针将引用有限对象的额外保证，转换由普通语法处理：

```c++
    // 创建一个共享引用
    TSharedRef<FMyClass> MySharedRef = MakeShared<FMyClass>();

    // 将共享引用转换为共享指针
    TSharedPtr<FMyClass> MySharedPtr = MySharedRef;
```

###### 那共享指针怎么转化成共享引用？

那共享指针只要**指向了一个非空对象**，就能够转化成共享引用。使用函数：`ToSharedRef`从共享指针创建共享引用。

> 断言（Assertion）是一种在程序运行时进行检查的机制，用于验证某个条件是否为真。如果条件为真，程序继续正常执行；如果条件为假，断言失败，程序通常会采取一些措施，比如抛出异常、终止运行并输出错误信息，以帮助开发者发现程序中的逻辑错误。

```c++
在解引用之前，确保共享指针有效，以避免可能出现的断言。
if(MySharedPointer.IsValid()){
	MySharedReference=MySharedPointer.ToSharedRef();
}
```

这段代码情景是关于共享指针和共享引用之间的转换，因为共享引用必须始终引用有效对象，所以当尝试从空共享指针调用ToSharedRef函数创建共享引用的时候，**断言机制**就会介入，通过程序终止或报错提醒开发者这里的错误，以避免后续因使用无效引用导致的问题。

#### 解引用和访问

可以像普通指针一样使用解引用

```c++
当共享指针非空时

if(node)
{
	三种方法都能够解引用节点，且对它的对象调用ListChildren
	node->ListChildren();
	node.Get()->ListChildren();
	(*node).ListChildren();

}
```

哇这个舒服了。不知道普通指针有没有get解引用方法，我去试试。可以的，。三种方式解引用都是可以的。

#### 自定义删除器

共享指针和共享引用支持他们对引用对象使用自定义删除器。如果需要运行自定义删除代码，要提高lambda表达式，作为创建共享指针时使用的参数。

什么时候使用自定义删除器呢？

默认的销毁行为无法满足需求，这时就可以使用自定义删除器。

```c++
like this:

void DestoryObjectType(FMyObjectType * ObjectAboutToDeleted){

	//添加删除代码；
}
TsharedRef<FMyObjectType>NewReference(new FMyObjectType(),[](FMyObjectType *Obj){DestoryObjectType(Obj);});

TsharedPtr<FMyObjectType>NewPointer(new FMyObjectType(),[]FMyObjectType *Obj{
    	DestoryObjectType(Obj);
});
```



## 共享引用

**共享引用** 是一类强大且不可为空的 **智能指针**，其被用于引擎的 `Uobject` 系统外的数据对象。此意味无法重置共享引用、向其指定空对象，或创建空白引用。



在温故一下，**共享指针和共享引用属于智能指针类型系统，不属于UObject系统。**

> 那我问你，共享指针是不是可以用于UObject系统内的数据对象？共享引用能用UObject中的数据对象嘛？



对于共享指针

可以用于UObject系统内的数据对象，但是通常不建议这么做，原因如下：

**生命周期管理冲突：**UObject系统有自己垃圾回收机制，其生命周期是由引擎垃圾回收控制的， 而共享指针使用引用计数管理生命周期，如果同时使用共享指针管理



## 弱指针：

尽量不要使用if(weakPtr.IsValid())这种代码，其存在一些弊端，那还能作为观察者模式对要观测对象的生命周期进行观察嘛？

- 在虚幻引擎中，在多线程环境下通过弱指针观测对象是否被销毁，更推荐的方式是使用`WeakPtr.Pin()`来获取`TSharedPtr`，然后直接检查返回的`TSharedPtr`是否有效

```c++
void ASmartPtrActor::TestWeakPtr_03(){

	printLog(TEXT("-----weakPtr_03------"));
	TSharedPtr<FSmartStruct>SmartStructPtr_01=MakeShared<FSmartStruct>();
	printLog(TEXT("SmartStruct_01 已创建"));
	
	TWeakPtr<FSmartStruct>WeakPtr=SmartStructPtr_01;
	//弱指针指向共享指针
	
	if(TSharedPtr<FSmartStruct>SmartStructPtr_02=WeakPtr.Pin()){
		PrintLog(TEXT("WeakPtr.Pin()获取共享指针结果有效"));
	}
	
	if(WeakPtr.IsValid()){
		PrintLog(TEXT("WeakPtr.IsValid()检查结果为有效"));
	}

}
```

正确使用方式就是使用一个变量接受Pin函数返回值判断是不是真，然后做后续结果



IsValid()函数结果是具有时效性的，**他只代表在调用时刻对象的有效性**。即使他返回true，也不能保证后续代码执行过程中对象仍然有效。因为对象生命周期可能会受其他代码逻辑形象而随时改变，这就要求开发者使用valid检查之后尽快使用pin()方法获取TSharedPtr再次检查其有效性，但是这样会增加代码复杂性。  要解决这个创建宏或者模板



> 奶奶的，别用弱指针监控其他对象是否销毁了，好麻烦，一般用来断开**[引用循环](https://zhida.zhihu.com/search?content_id=237369140&content_type=Article&match_order=1&q=引用循环&zhida_source=entity)**的问题就行了。





## 独占指针

```c++
TUniquePtr<FSmartStruct>uniqueP_01=MakeUnique<FSmartStruct>();

和C++智能指针相同，TUnique不允许拷贝或者赋值操作
    如TUniquePtr<FSmartStruct>UniqueP_02(uniqueP_01);
	 TUniquePtr<FSmartStruct>UniqueP_02= uniqueP_01;都是不被允许的
使用ue移动语义可以将独占指针资源所有权进行移动
        
         moveTemp（）
         
         TUniquePtr<FSMartStruct>unique_03=MoveTemp(uniqueP_01);
```







虚幻引擎的智能指针库提供了很多**辅助类和函数**，目的是为了使用智能指针时更加**方便直观**。主要有以下几种：

1  TSharedFromThis:要访问某个类实例中的this指针不建议直接返回this指针，因为this是普通函数，对其delete是合法的，这会导致很多不好的后果，比如指针被重复释放，操作野指针等，既然返回不了裸this指针就是将其包装成为智能指针返回。

（2）MakeShared（包括用于TUniquePtr的MakeUnique）：类似于C++的std::make_shared 。但是这个比直接用普通指针创建效率更高，因为智能指针包含两个部分，处了数据本身内存之外还有一个控制块，普通指针创建的时候会分别申请两次内存，而使用MakeShared的时候只需要一次。

（3）MakeSharedable,主要针对将普通指针转为智能指针，但是仍然不建议提前创建一个指针变量然后用来初始化智能指针。



**StaticCastSharedRef**和**StaticCastSharedPtr**：类似于C++中的 static_cast，但**只用于派生类的转换：**









**智能指针头文件源码（开头注释有很多关键信息）：SharedPointer.h**

**引擎自带使用示例：SharedPointerTesting.inl**









# 网络复制：



虚幻引擎有一个宏能够判断是不是属于服务端 switch has Authority

玩家蓝图上勾选replicates，能够在客户端拉取一些信息到客户端，勾选复制运动的话，每个端都会将运动数据上传，然后客户端从服务端获取。服务端还是自己的

对于一个箱子，服务端做函数按增减的话能够控制他的移动，这个主要逻辑是实现在gamemode中调用委托，而gameMode是只存在于服务器上的，所以客户端没法控制箱子移动， 所以由此引出了RPC调用，远程程序复制



虚幻有rpc框架  将事件中的