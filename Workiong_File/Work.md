# Actor Ticking 

TIck 每帧触发一次。Ticked 的顺序是基于`Actor World列表`中的顺序（主要根据Actor在场景中的创建顺序，添加顺序等）进行更新，tick分为三个阶段

三个阶段：异步处理前 异步处理中 异步处理后





## **Actor在何时被调用是根据什么控制的？**

**TickGroup**成员控制。

1. **TG_PreAsyncGroup**: 如果一个Actor需要在任何异步工作之前更新其状态，它将被分配到TG_PreAsyncWork组。如果不是怎可能会造成错误

2. **TG_DuringAsyncGroup**:可以与物理和其他线程并行调用的，被分配到TG_DuringAsyncWork组。如果这个组的任何Actor调用了一个处理刚体物理数据的函数，将log一个错误，并且忽略调用。这可能导致内存泄漏或者无效的场景状态。同样的情况也适用于SpawnActor(),moveActor(),SetLocation(),etc.这些函数会被继续执行，并且不会影响物理线程请问，同时也会打印错误。`也就是说，跟物理模拟等相关的逻辑可和物理逻辑一帧同时处理，但是Actor不要调用物理模拟的相关函数，不然可能会打乱物理模拟计算流程导致某些错误`

3. **TG_PostAsyncGroup**:最后，如果一个actor处于TG_PostAsyncGroup的话，他是依赖物理模拟的结果进行后续逻辑处理的，以便表示模拟后准确的世界状态。这个阶段调用所有与物理相关的函数都是安全的，因为，已经处理完包括物理模拟在内的所有异步信息了，不会感染异步线程中的处理逻辑与数据。

   

   

   ##  **Actor触发被分配至不同组的常用规则：**

   1. 可以改变碰撞的，生成可碰撞的actors的、写入Novodex数据的（这是个啥？）、那么这些Actor应该被放置在TG_PreAsyncWork组。常见的就是pawns、weapon、或者一些刚体类。
   2. 如果这些actor不能改变碰撞，或者生成non-collision的actors的 吗，或者不改变Novodex的。可以将其放在TG_DuringAsyncWork
   3. 如果需要物理引擎返回的数据，那么就是将Actor分在TG_PostAsyncGroup

   Novodex：物理引擎 Novodex data 就是物理引擎数据 

   

   ## **为什么物理模拟属于异步处理？**

   - 提高整体性能：
     - 独立于渲染线程：游戏中的渲染线程主要负责将游戏中场景元素绘制在屏幕，若渲染线程与物理线程放在同一线程同步处理，当物理模拟计算量较大时，占用大量时间，会导致帧率下降，画面卡顿，因此将其单独处理。
     - 与其他逻辑发分离开：游戏中有很多逻辑处理物理和渲染，还有剧情推进，玩家输入，Ai等计算行为。将各部分放在独立线程中处理，能够提高整个游戏的并行性与效率，避免不同逻辑之间的干扰和阻塞。
   - 实现并行计算
     - 如今cpu都是多核心的，将物理模拟作为异步处理，可将其分配到一个或者多个核心进行计算，与其他线程同时运行，充分利用性能
   - 便于管理与控制
     - 通常物理模拟的频率和游戏帧率不同，而且不同的物理场景和物理对象可能需要不同的更新频率，因此将物理模拟作为一部处理，可以方便独立的设置物理模拟的更新频率，根据实际情况进行调整 ，而不会受到部分游戏更新频率影响。
     - 错误处理和恢复：物理模拟过程中可能出现各种错误，如数值计算错误/碰撞检测错误., 异步处理的时候不影响其他游戏进程或者导致崩溃。

## **Attention:**

Actor列表的顺序和该Actor属于什么阶段处理没有联系，就比如一个Actor是PostAsyncGroup的，他也得等物理模拟等异步工作处理完了才被调用.



## Actor 生成：

 生成一个actor的时候，它和其所有组将会在生成时所在的组立即进行更新，而暂时不遵循原本设置的Tick Group规则。从下一帧开始，才会按照其自身的TickGroup组进行设置更新。（因为时被spawn的，所以刚开始的控制权不在与它，在下一帧的时候才夺回控制权）。有一个例外，在异步期间生成的actor会被延迟到TG_postAsync工作阶段。

Component：也会被分到不同更新组中，但是跟随所attach的Actor的更新组



## Tick的步骤：

遍历世界Actors列表，更新是TG_PreAsyncGroup的Actors,延迟其他组Actor的更新

对于更新过的Actors，遍历组件列表来更新TG_PreAsyncGroup中的Actors,延迟其他组Actor的更新(更新了actor上的组件)

-----------

启动物理模拟

更新TG_DuringAsyncGroup中的actors、

对于Actor中的每个组件，根据需要更新延迟的组件

更新延迟至TG_DuringAsyncGroup才更=更新的组件

Block 知道所有物理计算完成

--------------

更新位于TG_PostAsyncWork列表中的每个Actor。

更新Actor中的每个组件

更新每个延迟更新直到TG_PostAsycnWork出现的才进行更新的组件





# ActorComponent

组件是可以添加在物体上的一种actor 

primitiveComponent是一种可以被渲染和碰撞的组件

actor是有一个默认组件的。名为spriteComponent组件

组件的添加：Components.Add(组件名)；

代码如下：

这里的组件，是默认的组件

```javascript
class Actor extends Object;
var const array<ActorComponent>Components;
defaultproperties{

Begin Object 

Class =SpriteComponent Name=Sprite
Sprite =S_Actor
HiddenGmae=true
    
End Object
Components.Add(Sprite)


}
```



组件模板：就是我们平时开发创建的组件，就是我理解的组件”武器组件“”Action组件“等，用于挂载在Actor身上。

实例化关联：包含组件模板的类被实例化时组件也会实例化

继承特性：类的子类能够继承父类的组件，并且能够修改自身继承来的组件的属性。

序列化和更新：模板实例是根据模板默认属性进行序列化的/这意味着即使组件实例已经放置在游戏关卡中，组件模板属性变化时，组件实例也会同步更新
 **创建组件模板**

```
Begin Object 

 Class =SpriteComponent Name=Sprite
 Sprite=
```



#### 组件的序列化什么意思？

基本定义：将组件对象的状态信息转换成一种存储或者传输的格式的过程，这个过程可已将组件各种属性值，内部数据结构等信息以特定形式编码，在需要的时候能够创建出具有相同状态的组件对象

作用：

数据持久化：就是保存游戏，用于下次加载，恢复到之前的状态

网络传输：网络传输中需要将组件状态信息从一个节点传到另一个节点，比如网络游戏中，玩家角色的组件信息需要从服务器传到客户端，或者在分布式系统中不同服务器之间共享组件状态，序列化可以将组件信息转换成适合网络传输的格式。



#### 那UE的序列化是什么？怎么实现的？应用场合有哪些呢

Ue序列化定义：将ue中各种对象，包括但不限于游戏对象，组件，资产等啊，转换成字节流的形式，以便存储，加载，网络传输或者在编辑器中进行数据保存恢复等操作。



##### 怎么实现的？

Uproperty系统实现：通过UProperty宏标记属性，属性会被序列化系统自动处理。`UPROPERTY` 宏允许 Unreal Engine 的序列化系统识别并处理这些属性。

收起

Serialization函数：



由这里的Uproperty 联想到了其他的一些uclass ustruct ,uenum等都是一些宏

### 2025年2月14日记忆思路：

组件学习->组件可以序列化->序列化的定义->序列化在组件中实现的形式->uproperty宏标记组件属性->其他U开头的宏使用方法->为什么宏可以实现序列化->序列化、宏、反射之间的区别

**序列化、宏、反射之间的区别：**

**通过宏标记 实现反射的功能，反射能够实现的功能中其中包括序列化**

#### 预处理器宏：

包括： UProperty ,UStrcurt ,UClass, UFUnction, UInterface,UEnum,UDelegate,UMETA

##### 核心功能：

1. 是将 C++ 类标记为 Unreal 反射系统能够识别的类。宏为反射提供了必要的信息。
2. 引擎利用这些宏所标记的**元数据**，在运行时构建出类的详细信息，进而实现对类和对象的动态操作

##### 怎么理解这些预处理器指令呢？

UPOPERTY UFUNCTION UCLASS 等就像给代码打上了标签，告诉unreal引擎，哪些属性、类、函数需要被特殊处理。

##### 通过预处理器宏，引擎又能做什么？

Unreal Engine 能够在运行时了解类的结构、属性和方法等信息，从而实现以下几个功能,蓝图方面：**蓝图集成**。编辑器方面：**编辑器交互**，引擎数据处理方面：**序列化**

**蓝图集成：** 就是此时，引擎能够使用在代码中被“标记”过的函数，属性等，假如使用了UFUNCTION标记的函数，能够在蓝图编辑器中直接调用。Uproperty标记的变量能够在蓝图中实现读写功能。

**编辑器交互**：通过UPROPERTY标记过的属性能够在Unreal编辑器的细节面板实现显示和编辑，并且通过UMETA元数据，可以进一步控制属性在编辑器中的显示方式，比如，显示名称，范围限制，常见的就是editAnywhere 或者UMETA(DisplayName="myCustomeName");

还可以实现自定义操作，比如UFUNCTION函数能够作为自定义操作在编辑器中被调用，例如编辑器中添加一个按钮，点击实现调用特定C++函数完成场景布置或者数据处理



**序列化：**属性识别，通过UPROPERTY标记的属性是实现序列化的关键，引擎通过反射信息知道那些属性是需要序列化和反序列化的，哪些是不需要的 。例如

```c++
UPROPERTY(EditAnywhere)

float demo1;

float demo2;

demo1可以被序列化和反序列化，demo2就不行。
```

对象关系处理：UClass信息能够处理对象之间的关系。序列化一个对象的时候，引擎可以根据类的继承关系和其 成员变量的类型，正确的序列化与反序列化 对象和子对象（从来没想过使用UClass时候的继承关系中也能处理其子类的相关信息）





疑问： 宏所标记的元数据又是什么呢？假如UCLASS标记了一个类UHT是不是也记录了类中哪些变量被宏标记了，哪些没有被宏标记？

##### 元数据和UHT(Unreal Header Tool)

宏经过反射系统处理的时候会被UHT添加额外信息，这些信息被称为元数据.

不同宏标记会带来不同类型的元数据。

如UCLASS：标记类的时候，会提供类的基本信息，比如类名，父类，是否为抽象类以及宏标记中的参数等相关信息

如UPROPERTY:标记属性的时候，提供属性类型，访问权限，是否可以编辑，是否参与序列化等信息。

如

```c#
UPROPERTY(EditAnywhere,SaveGame) 
float demo1;
```

元数据可以获取 变量类型float、变量名 demo1、是否可在编辑器中修改 "EditAnywhere"  可以 、 是否可以序列化 可以，因为被Property标记了。

**从上面的uproperty的例子中，我们可以看出来，元数据保存的信息来自于哪里呢？**

**对象基础的类型信息** （例子中有变量类型，变量名） 和**宏标记的参数信息**（例子中的是可以在任何事情中编辑，可以序列化）这两大方面。



但是还能获取更多信息！就比如使用UClass的时候，除了和变量一样保存的信息能被UHT保存，其类的继承关系也能被保存，包括该类中哪些变量和函数被宏标记了，哪些没被标记，以及有宏标记的变量或者函数的相关信息。。。（也是套上娃了）



还有一个meta:`UMETA` 宏专门用于补充元数据。它可以与其他宏配合使用，进一步定制元素的行为和显示。

```c++
UPROPERTY(EditAnywhere, meta = (DisplayName = "Custom Name", ToolTip = "This is a custom property")) 
int32 MyProperty;
```

**那UHT生成的元数据这些代码在哪呢？会保存吗？**

会的，通常在.generated.h文件中





UHT 会生成额外的代码文件（通常是 `.generated.h` 文件），这些文件包含了类的反射信息。在运行时，引擎会使用这些反射信息来实现各种功能，如蓝图集成、序列化和动态属性修改等。所以，UHT 准确地记录了类中哪些元素被宏标记以及这些标记所带来的元数据，为反射系统提供了关键支持。







##### 宏对后续反射起什么作用呢？

为反射提供类型信息，然后当使用这些宏标记过的类、结构体、枚举时，UHT会生成额外代码记录这些类型的详细信息（包括继承关系，属性列表，函数列表，（假如UCLASS标记了一个类是不是也记录了类中哪些变量被宏标记了，哪些没有被宏标记？））

#### 反射：

一种编程机制 .通过反射系统，让引擎能够在运行时遍历对象的所有属性，并根据属性的类型进行相应的读写操作。这正是引擎能够**动态修改经过宏标记属性**的原因。

#### 序列化：

序列化是把对象的状态信息转换为可以存储或传输的格式（如字节流）的过程，反序列化则是将存储或传输的数据恢复为对象状态的过程。在 Unreal Engine 中，序列化依赖于反射机制。因为只有通过反射，引擎才能知道对象有哪些属性需要被序列化，以及这些属性的类型和值。例如，对于 `UMyClass` 对象，引擎会根据反射信息，将 `MyIntProperty` 的值进行序列化存储，在需要时再反序列化恢复该对象的状态。

#### 拓展：哪些情况下需要序列化？

1需要保存的数据，2网络同步，3自定义序列化

```cpp
    1 
	UPROPERTY(SaveGame)
    int32 MySaveableInt;
	2
    UPROPERTY(Replicated)
    float MyReplicatedFloat;
	3
    UPROPERTY()
    FString MyCustomSerializedString;

    virtual void Serialize(FArchive& Ar) override
    {
        Super::Serialize(Ar);
        Ar << MyCustomSerializedString;
    }
//在 Serialize 函数中，MyCustomSerializedString 虽然没有 SaveGame 说明符，但通过手动操作被序列化了。
```

#### 总结：宏是反射的基础，反射是实现序列化的关键

没有宏标记，引擎无法在运行时获取类的结构信息，反射也就没法实现。例如假如在编辑器中写的代码中一些类和属性没使用UCLASS和UPROPERTY，引擎就不知道UCLASS有哪些属性，也就没办法操作

反射在运行时能够让引擎遍历对象的所有属性，如果其需要序列化，则能够根据相关属性正确序列化。







# GameType

作用：明确游戏规则，制作游戏的时候，通常会有一个或者多个游戏类型使用的问题。比如死斗，夺旗，占点等类类型.

游戏类型可以决定游戏的规则，游戏如何开始，完成这个游戏满足的条件或其他游戏元素。

同时GameTypre也会根据不同的关卡进行改变，加载一个新关卡就加载一个新的游戏类型。该游戏类型中的某些逻辑从属于game type，比如将玩家固定生成某个位置，当玩家要进地下城而的时候保存玩家当前位置。来到地下城加载一个新的level和gametype.同时地下城的目标可能可主世界的2也不同，地下城的目标是杀死boss或者是收集物品等.当返回至主世界的时候，又会加载主要世界关卡，主要世界中将玩家生成在地下城入口，然后玩家继续执行主世界任务



所以gametype中这些游戏逻辑相当于在ue5中的游戏设置的蓝图。这些东西属于gameplay

在ue3中 通常需要在项目设置或者相关配置文件中指定GameType，然后再围绕这个Game Type编写相关的游戏逻辑代码，以实现特定的游戏模式



## GameInfo

作为数据容器和管理类。通过添加各种游戏属性和函数满足游戏对各种信息的管理和操作。



gameInfo类是游戏类型的基类。每次创建游戏都会创建对应的gameINfo类新实例，并将其分配为game type。

那与game type相关的更多的了解，需要理解gameLIfeCycle





gameionfo中负责管理游戏的整体信息，包括游戏模式管理、游戏玩家管理、关卡管理、网络管理等

gameinfo中包括很多以上相关类别的属性和函数

先不看具体的属性和函数，先学习游戏生命周期

##  gameLIfeCycle

uc的生命周期



### 初始化游戏 

**startup movies**开始播放启动动画（启动动画在初始化文件中）

**Map load** 地图加载（所有的游戏逻辑和交互都发生在map中，所以引擎启动的时候，他加载地图并load and  initiliza a  new gametype.然后开始match  ）-----match 是一个开始游戏的术语

**Entry Map** 游戏开始的时候可以在命令行指定加载的地图，如果没有指定，则加载默认地图。默认地图可以在DefaultEngine.ini中的[URL]部分设定

```c++
[URL]
Map=UDKFrontEndMap.udk
```

通常这个是加载的主菜单地图，比如什么开始游戏，读取游戏存档等

### **Main Menu**

没有特殊功能强制游戏使用特定的菜单作为主菜单，或者强制游戏在启动的时候进入主菜单。这个只是加载地图的任务之一。而且这个很灵活，你可以加载地图之后立即进入主菜单，也可以经过一段过场动画之后在进入主菜单。甚至可以完全掠过主菜单。、

### **Loading Screen**

从一个地图加载到另一个地图都会加载一段动画，这个加载实际上是从数组中选择的一段全屏视频

### **Game StartUp**

game 这个单词被用于很多地方，这个语境中，代表着加载地图之后的游戏玩法和一些新事件

### **Game init**

这个InitGame() 是当前Game info的一个事件，他在所有脚本被执行前调用，包括所有actors的PreBeginPlay()事件。

这个主要功能是为了给游戏类型设置它的参数以及生成任何所需要的辅助类



### **PreBeginPlay**

​	在Actor脚本执行的时候，首先被调用的脚本函数。这意味着他在gameplay开始前被调用，如果Actor在游戏启动的时候就存在，则就是此种情况，如果是游戏进行中被生成的，则在这里可以进行非常特殊的初始化操作，。在preBeginPlay的时候，组件还未进行初始化。

事件 event PreBeginPlay();



### **PostBeginPlay**

所有的actor都通过preBeginPlay事件初始化之后，才会被调用。该事件常用于属性初始化，世界中查找对其他actor的引用以及执行其他任何一般初始化擦欧总。可以认为这个是actor的脚本构造函数。



**这两个有什么区别吗？都是为了初始化，为什么不是使用一个初始化构造函不就行了，怎么还要两个不同的函数？**



有的

**执行时机不同**

PreBeginPlay:在Actor脚本开始执行时马上调用，是初始化过程中较早的执行函数。如果actor是放置在场景中或者游戏开始之前就存在的，那么该函数在游戏正式开始之前启动

PostBeginPlay:所有actor的prebeginPlay执行完成之后调用，此时处于初始化流程较后阶段

**可操作内容不同**

PreBeginPlay：执行事件较早，此时Actor组件以及其他外部对象都不一定完成初始化，如果需要查找环境中其他的一些对象进行获取某些信息初始化可能失败。此阶段只执行一些简单的，基础的，不依赖外部对象状态的初始化操作

PostBeginPlay：此时所有actor都完成了preBeginPlay阶段，所以此时整体初始化环境更加稳定，可以在这里进行更加全面复杂的初始化工作，例如初始化属性，查询场景中的其他actor引用并且建立关联等操作

**设计目的**

Pre是为了自身基础状态的初步设定，为后续打基础

Post 是综合初始化阶段，让actor在一个更加稳定的环境中进行初始化，



### **Player Creation**

玩家创建

分为三个阶段，PreLogin Logion PostLogin 

**Pre**是为了负责是否允许玩家进入游戏，可以使游戏类型的AccessControl对象确定玩家是否可以通过调用它的PreLogin()函数加入游戏

**Login()**负责生成玩家，应该在这儿里添加创建玩家需要的任何特殊功能

**PostLogin** 玩家成功加入游戏之后，开始对玩家进行初始化。同时可以安全调用playercontroller上的复制函数

`event postLogin(PlayerController NewPlayer)`



### **March Start**  

比赛开始，也是游戏开始



函数名为`StartMarch()`,在玩家初始化中进行调用的，也就是在`Post Login()`中进行调用，负责生成玩家的Pawns并且通知所有玩家比赛开始。(也可以从`Pendingmarch待处理比赛` 状态下的`startMatch()`和`StartArbitratedMatch()`调用)

`function StartMatch()`



玩家pawn的创建过程是实际发生在`RestartPlayer（）`函数中的，这个函数由StartHuman()和Start Bots()函数调用，而这两个函数又由`Start March（）`调用

*即：Startmarch->StartHuman()->RestartPlayer这样一层一层调用，生成pawns*

同时restartPlayer函数除了生成pawns之外还提供一个开始点的位置用于玩家开始

`function RestartPlayer(Controller newPlayer)	`

\

### **End Game**

在gameinfo子类中添加一些函数用于执行检查游戏是否结束的条件，当这个函数满足结束条件时，应该调用EndGame（）函数，EndGame函数同时又调用了checkEndGame()函数，确保游戏可以结束，然后执行与游戏结束相关的操作。



为什么会再次检查是否结束游戏？因为为了确保游戏结束的安全性和一致性，在 `EndGame` 里再次调用 `CheckEndGame` 进行最终确认。比如在多人游戏中，由于网络延迟等因素，可能会存在数据不一致的情况，再次检查可以避免误结束游戏。
