对话使用的struct 数据结构供datatable的使用，这样的datatable可以存储各种需要的信息，本次教程中仅使用了boolean IsNPC和string dialog



### 罗列下对话用到的相关蓝图：

首先是UI，玩家的Interaction组件，NPC上的datatable 增强输入转换逻辑



### 对话逻辑梳理

在player上的interaction 组件中有射线检测，射线检测到的actor保存为变量，然后再有dialog组件获取检测到的actor身上的datatable。然后再根据敌人身上datatable的信息进行显示提示按下空格继续交流，然后按下空格之后开始进行剧情推动 



### 为了实现逐行获取数据表中的对话信息：

dialog过程获取datatable，我们如果要使用的话，需要向将其存储为数组，然后再根据下标进行逐行显示。创建使用current Row，使其后续自加，然后实现逐行显示



### intercation交互组件

组件其实是为了显示一些EF交互以及射线检测的。和剧情对话的dialog要分开



### UI

Ui上的文本控件是能改变对齐方式的justification 如果想再一个文本框的左右两边分别显示文本的话，就使用这个



### 2025年2月16日

对话与任务系统进行绑定，并且在任务进行的同时，其他NPC应该有不同的反馈





剧情线对话，本质是流程图 

任务组件中应该有保存任务进度的地方。然后进行NPC的检测

### 如何实现线性任务？

前置：

Linear Mission数据表格：数据表中三个结构

 Rowname：对应处在任务的第几环。

missionNPC：该环任务需要交互的NPC

DialogTable:与该NPC交互时线性任务的对话信息



首先是在player controller或者是game instance中存储当前任务ID,然后每次检测这个任务ID

在与敌人交互的时候，通过检测任务ID获取一个数据表Liner Mission的表数据。

再判断是不是和missionNPC相同，相同的话再去显示线性任务的对话

如果检测到的敌人和表中要交流的敌人相同，则进行任务对话，并且任务index++，这样任务就会向下进行了



然后敌人身上的话，因为要将线性任务对话和非线性任务对话区分开，有不同的对话内容，所以我们需要一系列的datatable存放对话内容，那怎么管理这些datatable？

### 创建非线性任务的对话datatable

游戏主任务线中对应的有index，所以可以创建一个datatable只存储对话的dataTable，但是存储的是下标不等于主任务下标的内容，就比如跟吕布对话的主线任务下标为2，那么我创建一个datatable中只需要存放1和3的相关对话dataTable，如果任务进行到2的时候，就执行主线中的对话信息了



### 在主角MissionSystem中逻辑实现:

在MissionSystem的EqualLinearCurrentNPC中，当其中判断NPC的时候为false的时候，就用NPC上存放的属于任务线之外的对话table







# SmartObject



使用这个框架首先是使用 

#### gameplayBehavior

这个蓝图创建新的 蓝图 

**此蓝图主要作用：**

主要重载两个函数，一个是onfinished onTriggerCharacter

在On Trigger Character节点中获取avator 然后通过获取角色骨骼，就可以实现播放动画蒙太奇，结束或者打断的时候使用连接end behavior节点即可

#### gameplayBehaviorConfig

这个蓝图主要是用于配置，终于要在其中配置一下，behaviorClass就可以 就是选择上面的那个

#### 数据资产

数据资产创建选择smartObjectDefinition创建相关资产

#### 行为树：

行为数创建黑板，黑板中可以创建一个SO claim handle类型的黑板键即可

TASK1：寻找可以去的智能对象的位置。通过智能对象的系统 可以通过连接节点`find Smart Objects`用于在范围内搜索智能对象，可以把这个节点加在ai的pawn上，范围可以设置一下

![](..\Workiong_File\snpi\tree.png)

然后将搜索到的对象在数组中，通过判断数组是否不为空进行下一步







如果不为空，则在数组中随机选一个位置，将这个槽进行mark告诉其他ai这个槽被占领了

![](../Workiong_File/snpi/tree3.png)

最后将这个handle设置在行为树的黑板中即可

TASK2:在这个任务中，通过获取上个任务拿到的handle类型的黑板键就能对其进行移动到该位置的操作

]











# MASSAI



ECS框架处理AI

## MassEntity:

![mass02](..\screenshot\mass02.jpg)





该文件夹包括 mass原型数据 mass原型类型 mass命令缓冲 massEntitydebug mass实体设置，mass实体类型 mass实体视图，mass实体查询等

```v

一些功能的英语：
processor处理器； Utils 工具类 ；buffer缓冲区；  Archetype原型，典型； Module 模块 ；Fragment 片段； memory **Chunks**内存块

MassProcessor.cpp  ：mass 处理器\

“ProcessorDependencySolve”可能是与处理实体逻辑相关的功能模块名称，可能用于解决处理器（Processor）之间的依赖关系问题。在游戏开发中，当有多个处理实体逻辑的处理器存在时，可能会出现依赖关系，这个名称的模块很可能是用来处理这种依赖关系以确保游戏系统的正常运行。

MassSchematic.cpp：mass原理图

MassSettings.cpp：mass设置
```







> MassEntity is  a framework for data-oriented calculation 

#### 主要数据结构：

`MassEntity`的主要数据结构是`Fragment.`  `fragment`被视为组件 这些组件构成一个集合，这个集合的实例称为` Entity` ，这个`Entity`还能关联一个`ID`



创建一个Entity 和创建oop编程中一个实例有点相似，但是Entity不需要想类一样声明，而是由片段组合构成，这些组合可以在运行时被改变



-------

#### FMassArchetypeData:

定义的Entity数据的Archetype ,为什么叫原型，可以理解为类就是对象的原型，结构体就是结构体实例的原型，。因为游戏中要创建很多Entity所以需要有Entity的原型定义，可以描述内存布局等信息。

定义原型的时候需要四种信息作为参数（类似于构造函数？）

FMassFragment, FMassTag,FMassChunkFragment ,FMassSharedFragment

![img](https://pic1.zhimg.com/v2-a1ca7b487ecec1de0fc7d033107656d6_1440w.jpg)

**一般使用FMassFragment就好了**，就是定义每个Entity内部的数据结构 ，传统ECS中FMassFragment其实就是Component。实体是由片段组合构建的（Fragment Composition）,在运行的时候，是可以动态变化的。

-----

#### Archetype：

1. 实体和片段都是不包含逻辑的纯数据元素。 
2. `Archetype `（原型）是有相同组成结构的Entities的集合 。 不同的`Archetypes`由不同的片段组合，也就是有不同的组件。 所有与Archetype关联的实体（entity）都有相同的组件构成
3. 原型中的实体被整理在内存块（memory Chunk)中.这能确保从内存中检索相同实体的时候有最佳性能表现（**相同Archetype的entities是连续存储的吗？基本上是，但也有特殊情况**
4. 在游戏运行过程中，Entities 的状态可能会动态变化，**某些 Entity 可能会因为失去或获得某些组件而改变其所属的 Archetype**。这可能会导致内存中的 Entities 分布变得复杂，系统需要进行碎片整理等操作来优化内存布局，但这也不能保证相同 Archetype 的 Entities 始终是完全连续存储的。

![img](https://d1iv7db44yhgxn.cloudfront.net/documentation/images/1716a938-0605-46ef-a12e-fd9b98e19e63/me-ov-entity-diagram.png)

#### Processes：

提供逻辑处理的无状态类

![img](https://d1iv7db44yhgxn.cloudfront.net/documentation/images/f9f18ef6-4faf-4350-a144-abcd7397055a/me-ov-processor-diagram.png)





## Processing the Entities

#### MassCommandBuffer:

​	指令缓存 Processor可以通过修改fragements或者tags改变Entities的组合，而我们也知道，修改的时候，会改变entities的Archetype。

所以Processor可以使用MassCommandBuffer请求组合更改。这些命令可以将修改Archetype的操作暂存 这些命令在当前处理批处理结束时进行批处理，以避免上述问题。

#### EntityView

Fragment的序列化处理是 对Entities执行操作最有效的方式。（嘛意思啊）但是有时需要访问那些没有被处理的其他entities，Processors可以使用EntityView命令完成此操作

说人话就是：EntityView能访问不在处理队列中的其他entities

## Subsystems

 MassEntity分为几个子系统，所有子系统都是世界子系统，他们的生命周期和创建这各子系统的世界的生命周期绑定



#### MassEntity Manager

MassEntities实体管理器 is the most important part of the massEntity framework .该子系统用于create 和托管the Entity Archetypes

主要作用：这个管理器是处理entity的接口，比如增加减少fragments都是在这里进行的。他也负责在不同archetype间转移entities。其他子系统可以使用massCommandBuffer命令异步调用此功能





#### Entity Template

**实体模板**从虚幻编辑器中通过创建的masEntityConfig资产中的数据生成的，这些资产可以生生命一组创建实体期间添加到实体的特征Traits,    此外MassEntityConfig可以从父类资产中继承Traits，



#### Traits

定义：对提供特殊功能的Fragments 和Processors 的集体名称。可以向Entity添加多个trait实例，每个实例都在实现特定功能上发挥作用。

trait实例的作用：负责添加和配置Fragments，以使得Entity实现特定功能
