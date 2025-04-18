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
