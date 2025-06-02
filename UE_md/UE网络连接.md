# UE网络连接



### CS创建连接过程：

1. 客户端向服务端发送连接请求
2. 服务器在本地调用`AGameMode::PreLogin.`这可以使得GameMode有机会拒绝连接
3. 如果服务器接收连接->服务器向客户端发送当前地图
4. 服务端等待客户端加载完地图数据，客户端加载完成之后向服务端发送join信息
5. 服务端接收连接，则会调用`AGamMode::Login`.该函数作用是创建`PlayerController`.用于今后复制到新链接的客户端。成功接收之后`PlayerController`将代替客户端临时的`PlayerController`（也就是说客户端之前有一个本地的`PlayerController`做占位符）
6. 创建`PlayerController`之后，同时调用了`PlayerController`上的`BeginPlay`。
7. 一切完成之后将调用`AGameMode::PostLogin（）`.在PostLogin()之后，可以安全的让服务器在此PlayerController开始调用RPC函数

客户端请求->服务器PreLogin选择接收或者不接受->接收，则向客户端发送地图->等待客户端加载完成之后发送的join信息->服务器接收到信息之后调用Login()函数,创建PlayerController->完成之后调用PostLogin函数，之后服务器可以安全的RPC函数

通常服务器和客户端之间的特定信息的进行传输是通过服务器创建绑定的PlayerController当做桥梁的，PlayerController中包含相关的NetDriver,Connection以及Session信息。



### C上的Actor 连接

客户端上存在的一个Actor可以有连接也可以无连接，有连接时他的控制权限就是ROLE_AutonomousProxy,没有连接就是ROLE_SimulatedProxy。

但是客户端生成的actor一定与服务端没有连接。

网络连接的本质：只有PlayerController和Pawn能够直接和客户端建立连接(通过UNetConnection)。其他Actor若是想获取网络连接，必须和这两者建立关系

客户端创建的Actor无论是否设置Owner，都无法与服务器建立联系，因为：

- 客户端没有权限注册网络Actor，只有服务器能够通过NetDriver注册。
- Owner只是个引用，并不会自动赋予Actor网络复制的能力。

**为什么PlayerState/HUD能够通过Owner同步？**

因为这几种Actor是在服务端生成的。。然后复制到客户端的

##### 那如何实现客户端创建可同步的Actor呢？

唯一的方法就是Server RPC 请求服务器生成。



##### 那Owner的作用这这里面是什么呢？ 

网络编程中，Owner的作用确实容易跟网络连接混淆。

##### Owner的本质作用：

其实Owner是Actor的一个引用属性，用于表示创建或者负责该Actor的另一个Actor。

**核心用途：**

- 权限控制，决定哪些Actor可以调用该Actor的serverRPC 也就是函数要判断Owner是不是等于 xx类型玩家或者xx阵营的玩家。
- 生命周期控制。owner被销毁时，可以选择自动销毁其拥有的所有Actor。
- 逻辑关系：在游戏中创建Actor之间的父子关系武器->玩家

##### Owner与网络连接的关系:

关键点，Owner本身并不决定Actor是否有网络连接，也不会使Actor自动复制到客户端。即使将Owner设置为带连接的PlayerController，客户端直接生成的Actor也不会被网络识别

武器射击RPC的相关逻辑示例：

```c++
UFUNCTION(Server,Reliable)
void Server_Fire();

//客户端射击时调用
void AMyWeapon::Fire(){
	if(HasAuthority()||GetOwner()==GerWorld()->GetFirstPlayerController()){
		//服务器直接执行或者客户端通过RPC调用。
		Server_Fire();
	}
}
```

关于网络开发相关的代码规则：

###### **1. RPC 函数的声明（两端通用）**

RPC函数，只需要在头文件(.h)声明一次即可，用UNFUNCTION标记

```c++
//AMyWeapon.h
UFUNCTION(Server,Reliable)
void Server_Fire();
```

- Server表示这是服务器RPC,只能从客户端调用
- Reliable表示确保送达通过传输机制。

###### **2. 客户端调用与服务器实现的分离**

客户端调用逻辑

```c
//AMyWeapon.cpp(客户端和服务端共用)
void AMyWeapon::Fire()
{
	if(HasAuthority()){
		//服务器直接执行（无需RPC）
		Server_Fire_Implementation()//注意：直接调用实现函数
	}else if(GetOwner()==GetWorld()->GetFirePlayerController()){
		//客户端通过RPC调用服务器函数
		Server_Fire();
	}
}
```

服务器实现逻辑

```C
// AMyWeapon.cpp（仅服务器编译时包含）
void AMyWeapon::Server_Fire_Implementation()
{
    // 服务器执行的射击逻辑（如生成子弹、处理伤害）
    // ...
}

bool AMyWeapon::Server_Fire_Validate()
{
    // 验证调用合法性（如冷却时间、弹药量）
    return true;
}
```

###### **3.文件组织方式**

单一.cpp文件

利用条件编译分平台

```C
//AMyWeapon.cpp

//客户端服务器共用代码
void AMyWeapon::Fire(){
//..
}


//仅服务器编译的代码
#ifdef WITH_SERVER
void AMyWeapon::Server_Fire_Implementtation(){
    //..
}
bool AmyWeapon::Server_Fire_Validate(){
    //..
}
#endif
```

疑问：为啥没有Server_Fire()的实现呢？

有的，在虚幻引擎中，**服务器 RPC 的实现函数名必须以 `_Implementation` 结尾**，并且需要验证函数（`_Validate`）确保安全性。

整体代码可以如下：

客户端可以实现预测，就是先播放动画和声音....

```c++
void AMyWeapon::Fire()
{
    if (!HasAuthority())
    {
        // 客户端预测：立即播放动画和音效
        PlayFireAnimation();
        PlayFireSound();
    }
    
    // 调用服务器 RPC
    Server_Fire();
}
```

若服务器验证失败，会通过 `Multicast` RPC 通知客户端回滚预测。



```c++

// 服务器实现的射击逻辑
void AMyWeapon::Server_Fire_Implementation()
{
    // 验证调用者是否有权限
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
        return;

    // 获取射击起点和方向
    FVector StartLocation = GetMuzzleLocation();
    FVector ForwardVector = GetActorForwardVector();

    // 射线检测（服务器验证命中）
    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(OwnerPawn);
    
    if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, StartLocation + ForwardVector * 10000, ECC_Visibility, QueryParams))
    {
        // 处理命中逻辑（如造成伤害）
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->Implements<UMyDamageInterface>())
        {
            // 应用伤害（假设实现了 IDamageInterface）
            IMyDamageInterface::Execute_TakeDamage(HitActor, 10.0f, OwnerPawn);
        }
    }

    // 生成特效（服务器复制到所有客户端）
    SpawnMuzzleFlash();
    SpawnTrailEffect(StartLocation, Hit.bBlockingHit ? Hit.Location : StartLocation + ForwardVector * 10000);
}

// 验证函数（防止作弊）
bool AMyWeapon::Server_Fire_Validate()
{
    // 检查射击冷却时间
    return GetWorld()->GetTimeSeconds() - LastFireTime > FireRate;
}
```

- 服务器生成的特效（如枪口火焰、弹道）会通过 `NetDriver` 自动复制到所有客户端。







**Actor的ROLE你是知道的，但是ROLE为ROLE_Authority的时候就是服务端吗？**

并不是的，当在服务端创建了一个独有的Actor的时候他的ROle就是ROLE_Authority，所以应该通过获取NetDiver找到Connection来判断是不是服务端。（getNetMode()函数就是通过这样的方式实现的判断当前是否是服务端的）。对于服务端来说，其有N个ClientConnection，对于客户端来说，只有一个ServerConnection。

#### 三。RPC与同步

##### RPC和Actor同步谁先执行？

答案是不确定的，RPC的数据会立即进入SendBuffer中，而Actor的同步则要等到NetDiver统一处理。所以RPC是相对靠前的。不过因为丢包问题等，结果是不确定的。

这个问题造成的结果：

当创建一个新的Actor的时候，将此Actor作为RPC的参数传到客户端执行，可能会发现此时RPC参数为null、



一般什么情况下是UFUNCTION(Reliable, Client) ，什么情况下是UFUNCTION(Reliable, Server)呢？

**1. `UFUNCTION(Server, Reliable)`**

**含义**：客户端调用，服务器执行，确保送达。
**应用场景**：需要服务器权威验证的操作（如射击、交互、状态变更）。	

**2. `UFUNCTION(Client, Reliable)`**

**含义**：服务器调用，客户端执行，确保送达。
**应用场景**：需要同步到特定客户端的操作（如通知、特效）。



##### RPC参数与返回值

###### 参数：

除了UObject类型指针以及Const FString&字符串之外，其他类型指针或引用都无法作为RPC参数。对于UObject类型指针我们可以在另一端通过GUID识别，但是其他类型指针传过去是什么就无法还原其地址了。

*为什么其他指针 / 引用不能作为 RPC 参数？*

- **内存地址不可跨机器共享**：
  假设客户端有一个指针 `int* ptr`，指向本地内存地址 `0x123456`。当这个指针作为 RPC 参数发送到服务器时，服务器的内存布局完全不同，`0x123456` 可能指向无效区域或其他数据。因此，**传递指针地址毫无意义**。
- **引用本质是隐式指针**：
  引用在底层实现为指针，同样存在内存地址不可跨机器共享的问题。
- **UObject 指针是特例**：
  UObject 是虚幻引擎的核心对象基类，它有全局唯一标识符（GUID/ObjectID）。即使在不同机器上，UObject 指针可以通过 GUID 找到对应的对象实例，因此可以安全传输。

###### 返回值：

​		相当于一次性通知事件，如果再有返回值的话，CS来回进行RPC，每次都有返回值就无限循环了。

#### 四。客户端与服务器一致吗？

前面文件组织形式中我们已经清楚了，客户端服务端共用一套代码，但是写的时候依旧要提醒自己这些代码在哪个端执行。

#### 五。属性同步

 	首先我们要认识到，同步操作触发是有服务器决定的，所以不管客户端什么值，服务器觉得该同步就会把数据同步到客户端，而回调函数是客户端执行，所以客户端会判断与当前值是否相同来决定是否产生回调。

属性同步：基本原理，服务器创建同步通道时给每一个Actor对象创建一个属性变化表，当属性变化的时候，服务器都会判断新的值与当前值是否相同，如果不同就修改属性，并将新值同步到客户端。对于一个非休眠并保持连接的Actor，他的属性变化表一直存在，只要服务器变化，他就变化。

###### 属性同步的关键点：

1. 同步通道与属性变化表
   - 服务器为每个需要网络同步的Actor创建一个属性变化表，记录所有标记为Replicated的属性
   - 每个属性在表中都有唯一标识用于快速比较和定位
2. 变化检测机制
   - 脏标记：属性值被修改，服务器会设置对应的脏标记，表示该属性需要同步。
   - 值比较：同步前比较新旧值，值不同时才同步，以减少不必要的开销。
3. 同步流程
   - 服务器主导：属性同步完全由服务器控制，客户端只有接收和应用同步数据。
   - 定时同步：定时检查所有属性表变化，收集脏属性并打包发送至相关客户端
   - 可靠、不可靠 比如生命值通过可靠通道，频繁变化的（如位置）可以通过不可靠通道传输。
4. 客户端处理： 
   - 收到同步数据包之后直接进行更新。
   - 对于具有预测逻辑的属性，将会对比服务端的权威值与本地预测值进行比较，必要时进行纠正。
5. 休眠与连接状态
   - 非休眠Actor：属性变化表持续存在，任何修改都触发同步
   - 休眠Actor：暂时停止同步以节约带宽（如远距离物品）。唤醒时重新同步。
   - 连接断开：属性变化表销毁，重连时重建并同步。

###### 结构体属性同步

UE中UStruct类型结构体在反射系统中是UScriptStruct,其本身可以被标记为Replicated，并且其**结构体内数据默认都是Replicated的**，如果有子结构体依旧递归默认同步。

###### 属性回调

属性回调理论上一定会执行，而RPC函数在某些情况下可能不会执行，假如服务器上有一个宝箱，第一个玩家走过去，宝箱打开，如果使用RPCh函数执行多播可能会由于其他玩家距离过远，并没有将这个宝箱同步给此玩家客户端，那么这些玩家收不到RPC函数。当这些玩家过去后会发现箱子依旧是关闭的。

但是使用属性回调的话，第一个玩家接近箱子后将属性bOpen设置为true，通过属性回调执行开箱子操作。当其他玩家靠近箱子的时候收到属性bOpen，同时执行属性回调，这就可以实现，所有人靠近的时候都会发现箱子被开过了。

属性回调的核心优势在于**状态驱动的一致性**：无论客户端何时与 Actor 建立相关性，只要属性值被同步，回调函数就会执行，确保所有客户端看到的状态一致。相比之下，RPC 是**事件驱动**的，依赖消息实时送达，容易因网络或相关性问题导致状态不一致。

属性回调函数（如 `OnRep_XXX`）**仅在客户端执行**，服务器不会调用这些函数。服务器仅负责同步属性值。回调操作是客户端执行，所以客户端会判断与当前的值是否相同来决定是否产生回调。

对于宝箱的属性回调，其他玩家进入宝箱同步范围之后服务器会将宝箱标记为relevant（相关）。若玩家未收到宝箱数据，服务器则会创建一个初始同步包进行初始化操作。之后客户端创建宝箱实例执行同步属性的操作。然后客户端调用PostNetReceive（）函数，该函数检查条件，满足ReplicatedUsing和某些条件~~（如bOpen==true）~~之后，调用对应的回调函数，从而确保客户端能及时根据属性装填更新视觉表现和交互逻辑。

**场景 1：玩家 A 打开宝箱后，玩家 B 首次进入范围**

1. 服务器上`bOpen`已变为`true`。
2. 玩家 B 进入范围时，服务器将`bOpen=true`同步到客户端 B。
3. 客户端 B**首次接收该属性值**，无论值是`true`还是`false`，都会触发`OnRep_IsOpen()`回调。
4. 回调中根据`bOpen`的值执行对应逻辑（如显示打开的宝箱）。

**场景 2：玩家 B 已看到打开的宝箱（`bOpen=true`），服务器再次同步`true`**

- 客户端 B 的`bOpen`已是`true`，新值与旧值相同，**不触发回调**。

**三、为什么`bOpen==true`不是直接触发条件？**

1. **引擎无法预知业务逻辑**：UE 不知道`true`或`false`对游戏的具体意义，只负责同步值并触发回调。
2. **回调函数内部处理值**：回调函数（如`OnRep_IsOpen()`）中才会根据`bOpen`的值决定执行什么逻辑（如播放动画）。
3. **避免重复触发**：若每次同步相同值都触发回调，会导致不必要的性能开销（如重复播放动画）。



###### 为什么属性回调设计为客户端执行？

- 减轻服务器负载：服务器只处理游戏逻辑和属性变更
- 优化网络带宽：通过属性同步而非RPC调用，减少网络信息数量
- 确保客户端一致性：所有客户端基于相同属性值执行相同的回调逻辑，确保视觉一致性。

区分RPC和属性回调：

- RPC显式调用，请求客户端或者服务器执行函数
- 属性回调时隐式触达，属性同步时自动执行客户端逻辑。

###### UObject指针类型的属性同步

在传递一个UObject类型指针的时候，这个UObject在客户端存在吗？存在的话能够通过服务器的一个指针找到客户端上相同UObject的指针吗？

额能，通过FNetworkGUID.







#### 六。组件同步



静态组件 动态组件

对于静态组件：就是C++构造中默认挂载的组件或者蓝图中添加的组件。如果挂载此组件的Actor被标记为同步，那么其身上的静态组件也会被同步到客户端。 客户端虽然会得到一份组件，但是本质并不算同步服务器而是利用构造函数创建的。

动态组件，游戏运行时服务器创建或删除的组件，比如火把上的粒子组件。玩家靠近，说呢过后层火把生成一个例子特效组件，然后同步到客户端，玩家离开时，删除组件，客户端随之删除这个组件。

对于动态组件必须要Attach到Actor上并且设置其Replicated为true,即通过函数`AActorComponent::SetReplicated(true)`操作。设置之后组件在属性同步以及RPC上与Actor的同步就没区别了



从服务端发起的RPC和从客户端发起的RPC的区别。妈的原本的文字给我整的迷糊，这样就清晰很多了，发起这个词多清晰。

![从哪里发起的RPC](..\Snipaste\从哪里发起的RPC.png)

1. **“从服务器调用” 的主体与目标**
   - **发起方**：服务器（Server）。
   - **调用目标**：可以是其他客户端（Client）或服务器，具体取决于 Actor 所有权和调用类型（如 NetMulticast、Server、Client）。
2. **“从客户端调用” 的主体与目标**

- **发起方**：客户端（Client）。
- **调用目标**：可以是服务器（Server）、其他客户端，或本地逻辑，具体取决于 **Actor 所有权** 和 **调用类型**（如 NetMulticast、Server、Client）。