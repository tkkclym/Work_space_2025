# 弄清楚对一些常用类型的理解：



很多都是在引擎中经常出现，但是不是特别理解其作用和原理的东西

### Handle句柄：

> 句柄，英文原文handle，是计算机术语中翻译得最烂的词，（几乎）没有之一。

1. 意指通过其可以实现对于某种计算实体的识别或控制即可。
2. 这样说有点抽象，举个例子。door handle 是指门把手，通过门把手可以去控制门，但 door handle 并非 door 本身，只是一个中间媒介。又比如 knife handle 是刀柄，通过刀柄可以使用刀.
3. 用 handle 如何操作真正的资源，是实现的细节。

#### 那句柄的实现原理是什么呢？

好问题，首先想想句柄的类型是什么？

<img src="..\Workiong_File\snpi\局部截取_20250314_111058.png" alt="局部截取_20250314_111058" style="zoom:25%;" />

另一个FStateTreeDataHandle的就不放了，都是struct类型。



这个状态Handle是为了管理状态树中的众多状态.

1. **数据结构定义**

​		状态句柄通常以结构体的形式存在，其核心成员是一个索引（Index）,一般为unit16类型

- 为哈要用索引呢？这个索引作用是指向状态树里的特定状态。把索引类型设定为unit16，是为了在确保足够索引范围内尽可能减少内存。

2. **特殊索引值的使用**

​	表示一些特殊状态，，状态句柄定义了若干特殊的索引常量，如InvalidIndex，SuccessedIndex,FailedIndex和StoppedIndex

3. **索引有效性**

​	得确保索引是不是有效，是不是在有效范围内啊static bool IsValidIndex(const int32 Index)



4.  最重要的来了，句柄得具备状态转换和完成状态管理功能啊.通过ToCompletionStatus 和FromCompletionStatus 可以在状态句柄和EStateTreeRunStatus枚举状态之间进行转换

5. **底层存储与映射**

   句柄嘛，只是方便操作的工具。在引擎底层会维护一个状态数组或者其他数据结构，**状态句柄的索引就是这个数组的下标。**通过索引，句柄可以快速定位到对应的状态对象，这种映射关系使得状态句柄能够更高效地引用状态

6. **内存管理和生命周期**

​	句柄本身是一个轻量级数据结构，不直接管理状态对象的内存。状态对象内存管理由状态数系统负责，当状态对象被创建或者销毁的时候，状态句柄有效性会更新.例如，当一个状态对象被销毁时，对应的状态句柄会被标记为无效（`InvalidIndex`），避免悬空引用。







这个句柄为啥使用struct创建的还有其他类型吗？？

### Context上下文：



这里可以举一个状态数执行上下文的操作：State Tree Execution Context

状态数执行上下文是一个用于管理状态树执行过程的对象，他维护着当前状态和其他相关信息。状态句柄与状态数执行上下文交互主要体现在以下几个方面：

##### 获取当前状态

状态树执行上下文提供方法获取当前状态句柄，节点可以通过这个句柄获取当前状态信息。

```c++
FStateTreeStateHandle currentState=Context.GetCurrentState();
```





# 源碼學習：

### StateTreeType：

其实就是一个个”状态“的类，这里用Type表达类的概念。可以看作一个状态机的类。反正状态树就是一个个状态机之间的转换。



 分析下该StateTreeType的结构：

#### 首先是很多枚举类型【E开头的】：

**`EStateTreeStateType`**：**状态机类型。**定义不同类型的状态机。 State：普通状态机，Group:组状态机、Linked：链接状态机 、LinkedAsset：链接资产类型状态机

**`EStateTreeTransition Type`**：状态机转换的结果类型。定义转换的行为，None无转换，Succeeded成功结束，Failed失败结束等

**`EStateTreeTransition Trigger`**：**转换触发条件**。状态完成OnsStateCompleted 状态完成，OnStateSucceed状态成功。OnStateFailed转换失败，OnTick 每帧触发

**`EStateTreeTransition Priority`**：转换状态的优先级 。 





这个是FStateTreeDataHandle数据句柄指向的数据类型,是枚举类型E开头的

`EStateTreeDataSourceType`



#### 结构体类型：

**`FStateTreeStateHandle`** ：**状态树状态句柄。**表示状态树的状态，这个和状态机进行区分。提供了一些状态索引的管理和有效性检查等功能。  

**`FStateTreeDataHandle`**：**状态数数据句柄**。 表示状态树的一个数据，提供数据源类型、索引和关联状态句柄的管理和有效性的检查。

比如状态树中的一个状态机，。他的类型需要关联其他状态机，那么就是用这个句柄进行调用，句柄中有相关函数FStateTreeDataHandle,传入参数即可使用。



```

```

`struct STATETREEMODULE_API  FCompactStateTreeState`运行时展示在编辑器中的相关属性



### StateTreeExecutionType:



这个类很有用，适用于执行状态树的类，你看看他的文件名就知道是干什么的了，Execution。
FStateTreeExecutionFrame 这个结构体非常重要。它属于FStateTreeeExecutionContext的一部分，用于**FStateTreeExecutionFrame**





2025年4月18日

创建状态树的时候有两个父类可以继承创建，区别是什么？一个可以将AI Controller作为上下文，一个不行

![State的继承](..\Snipaste\State的继承.png)

![State区别](..\Snipaste\State区别.png)



### 土狗AI设计思路：

三次警告状态实现：

状态中设置一个定时器一次定时器结束之后刷新定时器，最后一次警告状态结束之后 对玩家进行攻击状态

接口：BPI_Mgr

管理类：BP_M_EgetW

当玩家切换武器立马调用接口将数据发送至管理器类中，敌人实时获取管理器中信息状态，然后通过管理器中数据转换状态树中状态

或者说状态树可以直接从管理器或者相关状态，然后直接转换？这样就省的在敌人蓝图获取一次？

**statetree可以绕过具体的enemy给敌人数据**，直接通过管理器获取信息，玩家信息只需要传递到管理器中

求值器只要是output，就能获取其对象内部变量

### 敌人相关具体动作实现

任务中获取挂载该状态树的行文很简单。

只需要创建一个Pawn类型的变量

然后设置为Input

若要实现相关功能

敌人身上实现接口即可。

然后在任务中调用接口



StateTree中在Event EnterState的节点后连接AIMoveto会造成卡顿，所以连接在tick事件之后，只有成功到达玩家身边才进行结束任务，合理！！



### 同一状态下，任务并行执行：

还有好像几个任务在一个状态中运行的时候会同时执行，导致我的敌人达不到我想实现的效果，所以注意这个问题

**在虚幻引擎的 State Tree 中，当一个状态中有三个任务时，默认情况下是同时执行的。**

State Tree 旨在提供一种并行处理任务的方式，允许不同的任务在同一状态下独立运行，从而实现更复杂和灵活的行为逻辑。每个任务都有自己的执行流程，它们可以各自独立地进行初始化、更新和完成等操作，相互之间没有严格的顺序依赖。

### 状态树怎么运行EQS?

我也是想的直接statetree任务中运行eqs，但是有问题了。

<img src="..\Snipaste\State运行eqs失败.png" alt="State运行eqs失败" style="zoom:50%;" />

runeqs是一个异步函数，所以说返回的是空的.

难道说要在StateTree中运行behaviorTree?

不，根据其为异步函数，还是可以解决的，引擎自带事件分发器，所以在使用的时候可以bind，当其完成的时候，调用自定义事件，然后通过返回的Locations中获取位置然后moveto即可

然后移动的时候需要将视角时刻保持在主角身上，tick函数中设置下即可



### 装备武器

**设置为StateTask**，因为被打的时	候与警告都要用得到，有一定的可复用性

装备武器是在Attention的时候开始调用接口，然后Enemy装备武器，

### 显示警告UI

在哪里显示？配表显示？？还是直接在状态树暴露参数显示？我的建议是配表存储在Pawn中，方便统一管理，对的，放在状态树中的话，如果多个ai用一个状态树的话，若是要显示datatable不一样就不好整了

### 放下武器之后UI还在显示

那就警告的时候，set visibility √  ，警告状态结束，调用接口，设为不可见





2025年4月27日

### escape

添加状态 escap 如何触发到此状态。攻击玩家时血量下降到某一阈值开始逃跑，还是说任何情况下，只要血量小于xx都逃跑呢？

攻击玩家状态时，血量小于xx转换为escape，因为砍一刀就能进入攻击状态。

##### 过程：

逃跑时调用Enemy的escape接口

escape接口实现将逃跑信息传递到Manager

Enmey再实现一个接收呼救信息的接口，创建一个同名任务。任务中确保Instigator！=self

状态树获取Manager中的结构体信息。

该信息在Move到目标点的时候使用。


### inverstigate

调查也只在巡逻的时候可以转换为调查状态。



通过中介者模式 其他npc发送信息到manager 监测此属性，若检测到则去调查。信息结构体。发送人，发送位置，。因为所有人都能接收，所以在接收的时候要判断发送者不是自己，不然可能导致自己刚发送又去调查了。

创建Condition 转移条件 ：C_toInvestigaor





在管理器random了一个police过去，那么意味着不用再进行审查是否是自身了，直接moveto



问题来了，当一个NPC看到我的时候另一个NPC也走过来了。设计的Mgr是不合理的。其不应该处理单独NPC的信息，而是处理几个NPC配合的信息

### 为什么调查之后没有回去巡逻而是一直在serach状态？

因为pawn的investigate还是true

在NOseeplayer的时候有一个tick监视isinvestigator这个值是否为true,为true则转换为serach状态，所以调查结束之后要将这个值置为false
