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

网络复制是什么意思？将一些信息从服务器到客户端的一些复制信息



虚幻引擎有一个宏节点能够判断是不是属于服务端 switch has Authority

玩家蓝图上勾选replicates，能够在客户端拉取一些信息到客户端，勾选复制运动的话，每个端都会将运动数据上传，然后客户端从服务端获取。服务端还是自己的

对于一个箱子，服务端做函数按增减的话能够控制他的移动，这个主要逻辑是实现在gamemode中调用委托，而gameMode是只存在于服务器上的，所以客户端没法控制箱子移动， 所以由此引出了RPC调用，远程程序复制



虚幻有rpc框架  将事件中的



# UE的网络概念

1. 是实时网络，就是相当于部落冲突那种弱联网而言
2. 不涉及HTTP和数据库（注册登录，增删该改查等）
3. 有Session(会话)概念，相当于房间
4. 适用于LAN和WAN，局域网，广域网

### 内容概要：

网络架构，网络复制，Ownship[联网的controller，pawn以及其所属的武器等], ActorRole， RPC，影响网络游戏流畅的因素

### 网络架构：

常规的Client - Server 架构

1. 一个服务端，一个或多的客户端   [与这个架构相对的就是端对端的网络模式]
2. 不能信任任何客户端，**所有信息都要经过服务器验证**,防外挂
3. Listen Server & Dedicated Server 【一人创建房间等待大家加入的服务器，监听服务器】 & 专用服务器【专有服务器只是用来计算，所有玩家都是作为客户端加入的】
   - 网络总会有延迟，如果是使用listen server 的话，开服务器的玩家肯定比别人更快
   - 打包dedicated Server  ,使用ListenServer模式开发打包的时候可以直接打包成dedicated Server ！！游戏框架逻辑几乎一样。只是打包方式可能有些不同
   - 后续学习基于listenServer
4. 当我们是客户端时是在操作本地角色还是远程角色？直接操作的是客户端，在服务端上复制本地操作的那个角色 就是远程角色。
   - 比如一个开枪操作，假如本地点击开枪，将请求发送到服务器，服务器进行验证是否有作弊行为之后再返回允许开枪，这个延迟是很大的，所以处理方式就是在向服务端发送请求的同时，本地就进行一次模拟开枪了。至于最终的结果是如何的都是通过服务器进行计算的，计算完再将射击结果发送的客户端。所以回到题目，我们在客户端激素hi同时操作了本地角色和远程角色。

![cs](C:\Users\atarkli\Desktop\snipaste截图\cs.png)

游戏状态以及每个玩家的状态都会从服务端复制到每一个客户端用于镜像，但是玩家的controller是不能相互知道的，只有在服务端有统一的信息，在每个客户端又有每个Client单独的Game Instance和UI用于处理本地的逻辑操作

#### 网络信息传递的主要方式:

replication   网络复制一定是单向的，只能从服务端复制到客户端

Rep_Notify

RPC  这个是任意方向

#### Replication:

网络复制  **只能从服务端复制到客户端**

Actor及其派生类才有Replication能力

Replication的三种类型：

​	Actor Replication  对象的网络复制

​	Property Replication 具体的属性的网络复制

​	Component Replication 组件本身的网络复制

##### Actor Replication  

两层意义：

1. 服务端生成，客户端也跟着生成一个镜像一样的东西，如果这个Actor没有开启replication,客户端不会有对象
2. actor replication也相当于一个**总开关**，包括当前Actor的所有属性的复制，组件复制，以及RPC的开关
   - 开启的话，在蓝图中勾选Replication C++中构造函数中将bReplicates=true
   - 如果没有开启，在服务端的一些物品就不会在客户端出现。
   - 然后如果生成的时候是在关卡蓝图生成的，可以使用Switch has Authority 根据是否是replication进行分支后续处理

![switch has Authority](..\Work_space_2025\screenshot\net\switch has Authority.png)

此时物体replicates关闭状态：这里是在服务端生成了一个物体，那么开始运行，客户端是看不到这个生成的物体的，但是！！！客户端操作的角色移动到这里的时候会被阻挡，为什么呢？ 

因为角色的运动是在服务端计算的，charactermovement组件，networking is fully implemented

如果物体replicates开启状态：则是服务端生成一个网格体复制到客户端一份。



##### Property Replication

属性复制。在蓝图就是将蓝图中创建的variable中的replication改为Replicated就可以实现复制了【这个是可以多选的，NONE,Replicated,Rep_notify】、

可以在蓝图中添加一个变量，然后通过选择不同的replicate进行实验

**C++：**

C++中如何实现呢？

1. 首先在类中添加网络复制相关头文件`#include "Net/UnrealNetwork.h"`
2. 在类中创建变量，变量要使用宏标记，UPROPERTY(Replicated).一般想要在蓝图中看到的话，再加个EditAnyewhere
3. .cpp文件中在GetLifetimeReplicatedProps函数中共添加DOREPLIFETIME(类名，变量名)。不需要声明此函数，直接重载添加

在C++中实现蓝图中has Authority逻辑的话：

```c++
.h文件：
public:
	UPROPERTY(Replicated)
  	  float Health;


.cpp文件：
   
void TESTActor::BeginPlay(){
	Super::BeginPlay();
	if(HasAuthority()){
		Health=500.0f; 
	}
}
void TESTActor::Tick(float DeltaTime){
    Super::Tick(DeltaTime);
    if(HasAuthority()){
        UE_LOG(LogTemp,Warning,TEXT("Server Health:%f"),Health);
    }else{
        UE_LOG(LogTemp,Warning,TEXT("Client Health:%f"),Health);
    }
}


void TESTActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(TESTActor,Health);
}

```

如果不需要网络复制的话，就把Replicated的宏删除，以及GetLifetimeReplicatedProps函数也删除，这样没有网络复制，LOG打出的服务端与客户端的值就会不同。



属性复制中的` DOREPLIFETIME(TESTActor,Health);`其实是有三个参数，第三个参数是条件复制，跟ownership相关





##### Rep_Notify 复制通知

​	意义：一个变量设置为Rep_Notify ,当该变量发生复制时，服务端客户端都可以调用一个自定义的函数 。也就可以设置为当这个值改变的时候，客户端服务端都有调用一次这个变量对应的Rep函数

https://www.bilibili.com/video/BV1ht4y1z79w?t=364.1

但是如果是**C++**，仅在客户端调用函数

C++中实现Rep_Notify机制：



- C++新创建一个Actor
- 然后将Actor的网络复制打开(总开关)，也就是bReplicates=true;
- 声明一个变量`UPROPERTY(ReplicatedUsing = OnRep_Armor)float armor =0.0f;`
- 声明通知函数` UFUNCTION() void OnRep_Armor();`
- Tick中刷新下数值，数值更改使其调用OnRep_Armor函数。

```c++
void MyActor::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if(HasAuthority()){
		Armor=FMath::Rand();
	}
}
```

- 定义通知函数OnRep_Notify

```c++
void MyActor::OnRep_Armor(){
	if(HasAuthority()){
		UE_LOG(LogTemp,Warning,TEXT("server Armor:%f"),Armor);
	}else{
		UE_LOG(LogTemp,Warning,TEXT("Client Armor:%f"),Armor);
	}
}
```

- 加上变量复制注册函数：

```C++
void MyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(MyActor,Armor);
}
 
别忘记网络相关头文件 #include "Net/UnrealNetwork.h"
```

如果报错没有根场景组件的时候，那就在构造函数中添加一个根场景组件就行

```c++
SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
```



然后正常运行时我们就会发现，只有打印出来的Client Armor，这个就是起初提到的**C++使用此机制仅在客户端调用函数**，后面RPC的话可以调用服务端的函数



#### OwnerShip所有权



每一个接入服务端的玩家都会创建一个连接，也就是connection,然后这connection就会分配一个player controller 用于控制相应的pawn等，这就是从connection形成一个关系链

![ownership](..\Work_space_2025\screenshot\net\ownership.png)

###### 为什么要了解ownership？

1. RPC要确定哪个客户端将执行运行于客户端的RPC 【important】
2. Actor复制与连接相关性
3. 设计所有者时的Actor属性复制条件

ownership会在actor复制期间使用。acitor有一个属性bOnlyRelevantToOwner.谁拥有这个开启了这个属性的actor，谁才会接收到这个actor的属性更新。 而player controller是默认开启的，也就是其他玩家没必要获取其他controller中的信息。    比如控制的pawn的位置不能让其他玩家知道吧，比如我的弹夹中有没有子弹，有没有空投枪啥的不能让其他人知道吧，尤其是搜打撤游戏中，携带物资多少尤其不能让其他人知道。

 

###### 如何设置或/改变/获取 ownership？

- spawnActor中有owner

![spawn](..\Work_space_2025\screenshot\net\spawn.png)

- setOwner:节点直接使用搜素就能使用

- Possess（onpossess>possessedBy>setOwner),Unpossess，这个在AiController中用过，拥有一个pawn或者取消拥有时使用。

- GetOwner 用的比较多了，动画通知，动画通知状态，组件等中都使用过。主要作用通过获取拥有着进行调用某些相关蓝图接口，实现相关功能。

#### Actor Role

> 在 Actor 的复制过程中，有两个属性扮演了重要角色，分别是 **Role** 和 **RemoteRole**

- Authority 权威
- Simulated Proxy   模拟代理
- Autonomous Proxy  自主代理

![roel](..\Work_space_2025\screenshot\net\roel.png)

- 不管是哪个角色在服务端的话都是Authority。
- 有了这role 和remoterole就能知道谁拥有actor的主控权，actor是否被复制，复制模式

首先就是谁拥有特定的actor主控权。要确定当前运行的引擎实例是否有主控权，需查看当前Role属性是不是ROLE_Authority.如果是，就说明这个运行中的引擎实例掌管此Actor(决定其是否被复制)

嗯，，感觉就是定死的嘛，服务端authority,而客户端中是另外两个。就是服务端有主控权，并且只有服务端能够向客户端单向同步呀。



###### 复制模式：

服务器不会在每次更新时复制 actor。这会消耗太多的带宽和 CPU 资源。是有时间间隔进行发送请求然后返回请求的，不然数据量交互太大。假如A在移动，B端观察那个A的时候有可能是一卡一卡的，而游戏中没有这种情况，原因是有移动位置之间是由插值存在的，具体怎么解决呢？就是两种模拟方式嘛，也就是对应的不同的模拟方式`ROLE_SimulatedProxy`与`ROLE_AutonomousProxy`



### RPC 

> 终于来了，RPC 网络复制说完就说RPC

#### RPC是什么？

函数调用，但是不一定是本地执行，而是调用远端函数

- 客户端调用服务端执行
- 服务端调用客户端执行
- 函数**不可以有返回值**
- 默认是不可靠的，如果要让他可靠得加个reliable 嗯，看公司项目里还分为reliable server 和reliable client

##### 为什么RPC不能有返回值？

1. 网络通信的异步性：RPC调用是异步的，客户端发送请求之后不能立刻得到结果，服务端需要处理时间，返回值无法及时传递
2. 网络可靠问题：网络传输可能丢包或延迟，返回值可能无法正确达到调用方
3. 执行顺序问题，多个RPC可能乱序到达
4. 性能考虑，返回值可能增加网络流量和同步复杂度



##### RPC没有返回值，那是如何实现功能的呢？

1. 服务器通知：服务器通过单独的RPC通知客户端结果（如ClientEndSkill）
2. 状态同步：通过变量复制(replication)同步状态（如Available变量）
3. 回调机制：通过事件或委托通知调用方结果
4. 预测机制：客户端先本地执行，服务器再验证

#### RPC蓝图/C++实现

##### 蓝图：

customEvent的RPC选项设置为：

Run On Server

Run On Owning Client

Net MultiCast



前面说了RPC函数是不能有返回值的，所以在创建的蓝图中怎么设置呢？

通过创建Custome Event，然后在细节可以选是否开启RPC以及是否是reliable

![rpc1](..\Work_space_2025\screenshot\net\rpc1.png)



假如自定义事件是onserver的时候，客户端触发这个事件就会在服务器上进行逻辑操作。客户端触发了，服务器是有这个客户端的authority的，客户端的玩家也是服务器复制来的。客户端调用服务器事件的时候是在服务器进行逻辑处理，然后再将效果复制到客户端的。

还有个点是需要注意的，当在playerController上使用RPC的onserver的函数时，只会在当前玩和服务器上运行，也就是controller的函数内容其他玩家是接收不到的，也不该接收



Net MultiCast是只在服务器执行的，用的比较少



##### C++：

要将函数声明为RPC的话只需要要将

Server

Client

NetMulticast 

关键字添加进UFUNCTION声明中。【额，还是和uc脚本有些区别的，正常】

engine


