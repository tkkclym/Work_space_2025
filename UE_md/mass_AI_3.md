### AgentTrait.h中两个类：

#### UAgentTrait 类：

Trait中创建fragment和共享fragment。[都是结构体类型USTRUCT宏标记]

共享片段在类中创建变量声明即可。

fragment需要的是在build ,build就需要虚函数·`virtual void BuildTemplate（FMassEntityTemplateBuildContext& BuildContext ，UWorld& world ）const override;`



Fragment中的结构体内容分析：

Inventory：库存，存储的是树和石头的资源，后期建造的时候，会有一个处理器对建造时的实体中的Inventory中的数量减一下。看清楚其结构体，是一个map.键为枚举值，值为int类型的数据



ResourceHandle:资源智能对象的句柄，采集资源的时候智能对象激活的时候会Reset一下，详见GatherBD中的Active函数 Agent。ResourceHandle.Reset();



#### UAgentInitializer 类:UMassOserverProcess  

此类继承的是UMassOserverProcess .也就是，这个类一般用于初始化，也可以监控某些Fragment的增删改查或者tag的改变来对实体初始化。【注意：EntityQuery必须为Processor的成员变量，可有多个entityQuery。其在构造函数中注册到Processor中。】所以根据注意，这个ObserverProcessor也需要有Entity Query作为成变量

​	此类中需要重写函数Execute,configureQueries 和Initialize,这几个函数，主要功能是做什么呢？

##### Initialize

​	初始化中，因为这个类是observerProcessor类，所以既然要监控某些片段，那就要初始化时声明要监控谁，即：·`ObserverType=FAgentFragment ::StaticStruct();`

观察他的什么行为呢？`operation =EMassObservedOperation::Add`



##### Execute函数

我们的目标是实体采集场景中的东西然后实现进行构建建筑，这里Execute函数是将实体都加入子系统



##### ConfigureQueries函数

​		想想处理器应该配置什么呢？

​		他其中的EntityQuery在这里添加了片段，共享片段，Tag，以及最后添加了一个不知道什么东西，EFragementAccess

### ConfigureQueries函数作用是什么？

通过 `ConfigureQueries()` 配置，明确了 `UItemInitializerProcessor` 处理器执行逻辑时依赖的实体数据片段，确保只有包含指定片段的实体才会进入 `Execute` 函数处理流程，同时规范了处理器对各数据片段的访问权限（读写或只读），保障数据操作的合理性和安全性。

通过ConfigQueries的配置，明确了UItemInitializerProcessor处理器执行逻辑时依赖的实体数据片段，确保只有包含指定片段的实体才能进入Execute函数处理流程，同时规范了处理器对各个数据片段的访问权限，读写或者只读，所以应该书写顺序是initialize，ConfigureQueries,execute，这样流程书写

### 建造子系统中

头文件包含一个结构体FBuild

其中一个智能对象handle:	BuildingRequset:建造请求 ，一个是需要的楼层FloorsNeeded



一个建造子系统类UBuildingSubSystem

​	类中函数挺多，不一一列举了，类中也有多个变量包括实体类句柄，Fbuild数组，实体类数组，需要的资源数组







> 看视频还有一个ItemTrait，这个是啥呢？也是一个trait。

这个怎么理解？应该就是资源的相关位置等信息？然后fragment中是相关的资源类型，也就是树啊，石头啊等，添加到总的Trait配置表中，作用就是每次带什么item，带多少个。



类: UItemTrait : UMassEntityTraitBase

结构体： FItemFragment :FMassFragment

结构体： FItemAddedToGrid :FMassTag

类：UItemInitialzerProcessor : UMassObserverProcessor

​			这里初始化的execute作用就是，如果位置为0，那么就将oldLocation做数学运算随机生成

类：UItemProcessor :UMassProcessor





### 资源类：

Resource.h Resource.cpp

​	创建C++资源基类之后还需要到引擎中创建两个子类，石头和树。 C++资源类中需要静态网格体变量， 函数的话，一个·`AddToResourceQueue` 这个函数作用是什么？将资源类的智能对象句柄添加进建造子系统的智能对象中。【疑问：这个函数在哪里被调用了？】



### 资源类的智能对象行为定义C++:

GatherBD：智能体需要的行为定义。

需要实现的是Active 和DeActive

这个整个逻辑就是激活的时候重置资源状态，失活的时候，触发实体生成【树桩子，碎石头等】,然后就是把智能对象进行销毁。、



#### 这个智能对象的definition

​		这个是继承自mass相关的智能对象Smart Object Behavior Definition，和普通的智能对象作出区分，以及，其中有需要作为公开变量的话，配置是在如图所示的地方实现的。可以看到ItemConfig是需要创建的数据资产。



<img src="C:\Users\atarkli\AppData\Roaming\Typora\typora-user-images\image-20250401210408486.png" alt="image-20250401210408486" style="zoom:33%;" />

这里居然需配置相关的Trait,我没想到，但是也符合ECS思想。<img src="C:\Users\atarkli\AppData\Roaming\Typora\typora-user-images\image-20250401210735238.png" alt="image-20250401210735238" style="zoom:33%;" />



其中DA_Entity是移动的Mass的配置文件，也就是一个是采集的，一个是资源，都是使用的ecs思想搞清楚；

### 建筑类：

Building.h Building.cpp

建筑类也是智能对象，需要完成的目标是，获取资源之后0.5秒添加一层，所以绑定一个delegate获取信号。

总体逻辑：

​	构造函数：构造的时候嫁给你智能对象组件添加进场景组件。

​	beginPlay函数：

​		绑定委托和时间句柄，在接收到委托之后经过0.5秒之后继续执行逻辑

那接收到委托之后的增高逻辑哪里执行的？~~好像是Construction中实现~~**调用建筑子系统中的addBuilding函数**



然后蓝图中继承这个C++，接着添加一个SO组件，整四个Slot,整完去整SOBD，SOBD就是下面的



### 建筑SOBD ConstructionLevel:

类名**ConstructioinLevel**和资源类的行为定义差不多；确定了也是Smart Object Behavior Definition





### 另外新建一个文件Construction :

### 类Construction :UMassObserverProcess

这个类继承与MassObserverProcessor

- 结构体USTRUCT  : FConstructionFloor:使用Tag标记建筑的楼层.**这个结构体是标签**。。。`FMassTag` 及其派生结构体（如 `FConstructionFloor` ）作为标签使用时，设计成空结构体有特定原因和好处，这并非特殊用法，而是符合框架设计理念。首先是数据轻量化，标签的主要作用是标识实体，用于标记实体是否具有某种属性，状态，或者某个类别等，它不需要携带复杂数据仅做为身份标识，所以空结构体足以满足此条件。
- 类Construction   其中函数： Execute ConfigureQueries Initialize 变量：SmartObjectSubSystem ,IncrementHeight 还有一个EntityQuery

联动之前的代码解释就是

构造函数中执行的 为为观察类型赋值，以及操作类型为Add添加操作

## 状态树中：

状态树的任务是Struct类型，为什么不是Class？好像不是很重要

任务中添加必要的变量和函数：



任务中主要的三个函数：

Link链接外部变量

EnterState:进入状态

Tick：每帧执行



Link就是状态树对外部资源进行链接时调用可以解析其他状态树的状态引用。



状态树源码结构，分为C++方面的和BP方面的，



C++的Condition，task,Evaluator的基类为StateTreeNodeBase.h



# Dialogue



Dialogue中在项目中放置插件，Rider打开插件发现插件没有索引的解决办法：

- 打开项目根目录下的 `.uproject` 文件，用文本编辑器打开。
- 检查 `Plugins` 部分，确保你的插件配置正确，例如：

```json
"Plugins": [
    {
        "Name": "YourPluginName",
        "Enabled": true
    }
]
```

- 若插件配置缺失或者有误，进行相应的修改，保存文件后重新生成项目文件。

### 然后在rider中构建一下索引就ok了

