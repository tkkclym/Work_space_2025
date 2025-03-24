# UObject(一)



UObject提供元数据、反射生成、GC垃圾回收、序列化、编辑器可见、Class Default Object 等。UE可以构建一个Object的世界。



引用UObject作为根基类设计有什么深远影响呢？

使用它我们能得到什么？

### 优点

1. **万物可溯源。**有了一个统一的UObject,我们可以根据一个object类型指针追踪到所有派生对象。如果愿意，我们甚至可以将所有的对象都遍历下来。按照纯面向对象的思想，万物皆对象，所以一个基类Object会大大方便管理。
2. **通用的属性和接口**。 在继承机制的特性下·，。我们可以在object中加上我们想应用于所有对象的属性和接口，比如getName，clone等，代码只需写一遍所有对象都可以应用上
3. **统一的内存分配释放。**Cocos里使用CCObject就是如此实现的，但是实现的不够好，UE的工程师是比较牛逼的。使用垃圾回收方案的话，你需要一个统一Object可以引用，所以这也是为什么几乎所有支持GC的语言都会设计一个Object基类的原因了。
4. **统一的序列化模型。**如果想让系统中各个类型对象支持序列化，要么针对各种类型都写一套序列化方案，要么就是**利用宏和模板标记识别**。最差就是利用继承机制把统统一序列化代码放在Object中，如果有设计良好的反射机制，实现序列化就更方便了
5. **统计功能。**能够了解整个程序下来哪个对象分配最多次，哪个对象分配时间最长等等，反正因为可追踪，所以这些功能也就好实现了。
6. **调试便利**。如果一块泄露了内存的地址，如果是多类型对象。你不知道这块地址的类型，你都不不知道泄露的是哪个对象，**但是现在你知道了，他是object下的一个子类对象**，将地址转换成一个Object的指针，然后就可以一目了然的查看对象属性了。
7. **为反射提供遍历。**如果没有统一的object，很难为各种对象实现GetType 接口。否则你就得在每个子类里都定义实现一遍，用宏也是治标不治本。       反射的作用： 运行时能够获取对象的类型信息、属性、方法等元数据的能力。UI编辑中反射是很重要的，它允许编辑器在运行时动态的获取对象的属性等信息，以便在UI的属性面板中显示和编辑这些属性。
8. **UI编辑的便利。**为啥Object和ui便利有关系呢？和编辑器集成的时候，为了让UI的属性面板控件能够编辑各类对象，不光需要**反射支持**，还需要引用一个统一的Object指针，否则如果用一个void *Object ，你还得额外添加objectType枚举来转换成正确类型的C++对象，而且只能支持特定类型的C++对象。

![obkject](..\Work_space_2025\Workiong_File\snpi\obkject.png)



### 说完优点了，说说缺点:

1. **显而易见的臃肿。**继承的祖传毛病。越想为所有对象提供额外功能。就越会在object中堆积大量的接口函数和成员属性。久而久之。Object就挂上了各种代码。
2. **不必要的内存负担**。有时候有些属性可能很久都用不到，但是为了所有对象在用的时候都有，还是不得不放在Object中，目前还不知道object中都放置了哪些必要的属性/、
3. **多重继承的限制！**！。C多重继承自A和B，如果A和B都不是Object还好，但是此时A，B都继承于Object那么就变成菱形继承了。甭管用上用不上全部用虚函数显然不靠谱，所以一般有Object的编程语言，都是限制多重继承。改为多重实现的接口，避免数据被继承多份的问题。
   - 问题来了，**多重实现接口**是什么呢？是**解决多重继承问题的替代方案**，接口是只包含函数声明没有具体实现的抽象数据，一个类可以实现多个接口。
   - 在这里使用接口的优势：
     - 它与多重继承不同，它不包含数据类型，只定义行为，当一个类实现多个接口时，不会出现因为数据重复继承而导致的歧义
     - 提高代码灵活性和可维护性。接口提供松耦合的方式定义类的行为，一个类实现多个接口实现那不同行为特性，同时这些行为之间关联性和独立性可以很好的控制和管理
4. **类型系统的割裂**，这里说的是UE并不像Java和C#那样对用户隐藏掉了背后系统。用户在面对原生C++类型和Object类型的时候不得不思考怎么划分对象类型。两套系统在交叉引用、互相加载解释、消息通信、内存分配的时候采用的机制和规则是不大一样的。哪些对象应该继承于Object，哪些不用，哪些可以GC,哪些只能用智能指针管理；C++对象new了object对象怎么管理，Object对象new了C++对象什么时候释放，这些都是强加给用户思考的问题。



### 权衡

> 但是设计是权衡的艺术。-- 沃斯基

1. sizeof(UObject)= 56 56个字节还可以接受

2. **规避多重继承**

想要在C++中实现功能复用，这里提供两种方法解决规避多重继承：

- 采用组合的组件模式
- 把共同逻辑写在C++类型中，比如UE中F开头的很多类就是这样的功能类。

3. 多学习喽，UE之路就是登神长阶。UE提倡BP中实现游戏逻辑C++充当BP的虚拟机，这样就可以对用户隐藏掉复杂性。另一方面C++在UObject上也提供了大量的辅助设计，如UClass等各种宏的便利。newObject方便接口，UHT自动分析生成代码，避免用户直接涉及UObject的内部细节。
4. 信任感的建立很重要，当使用UE的时候一旦对象被释放掉或者没有释放，第一时间应该想的是自己的使用问题。



# UObject(二)类型系统概述

> 说实话大钊文章读着真的妙趣横生，早知道先看文章再去b站油管啥的找教程了，这又系统又有趣的文章为啥没早点看？？相见恨晚



### 引言：

探究UObject应该先学什么呢？他有那么多特性，序列化，反射，GC,编辑器支持。。  

那个是必须的？GC吗？不是，大不了自己new,delete 或者使用智能指针管理对象生命周期。序列化吗？手写对象的数据排布，麻烦是麻烦，也能实现。编辑器支持。默认类对象，统计都是之后的额外附加功能。

而反射，是必要中必要。大多数游戏引擎没用反射，不是一样过的好好的吗？确实，不利用反射功能，不动态根据类型创建对象，不遍历属性成员，不根据名字调用函数，大不了都是绕一圈，没有过不去的坎。

但是我们既然已经论述了统一Object模型的好处，如果不使用反射的话，就好像砍断了Object的翅膀。还有就是反射作为底层系统，实现完善了也可以大大裨益其他系统的实现：

- 比如有了反射序列化实现起来很方便；

- 有没有反射GC实现时的选择方案完全是两种套路

  - 举个例子：反射机制会为每个类创建一个对应的元数据对象（可以将理解为Class对象），这个元数据对象包含了该类的各种信息，如类名、属性、方法等。同时、为了方便管理和跟踪对象实例，这个class还可以维护一个**列表**,用于存储所有该类对象的对象指针，这样在程序运行过程中，只要知道了类的信息，就能通过对应的class对象（中的列表）找到所有该类的实例对象

  ![uclass](..\Work_space_2025\Workiong_File\snpi\uclass.png)

  

  - 在垃圾回收中，垃圾回收器可以通过class对象的objectReferences列表快速定位到所有该类对象，然后检查这些对象是否有其他对象引用了他们，如果没有引用了，就可以标记为可回收对象，从而提升垃圾回收效率。
  - 垃圾回收差不多了解了，先学反射，垃圾回收放后面仔细研究

  

这里大钊给了个C#的反射示例，让我们大概了解下反射中都有什么时吗？首先是dll库（程序集），dll库中分moudle就是对应模块，模块中包含的就是Type也就是上面说的存元数据的对象，其中包含的信息不少呢，构造函数啊，event事件啊成员变量啊，函数，函数参数等等很多信息。这里给C#的例子就是让我们知道，代码定义出来的类型，可以通过一个数据结构完整描述出来，最重要的是可以在运行时得到。



### 类型系统

​	虽然上面说了很多反射的术语描述我们熟知的运行时的类型信息的系统，**动态创建类对象**等，但是其实“反射”是在“类型系统”之后实现的附加功能，不要只看反射而忘记朴素的本质支撑。

​	想想看，如果我实现了class类提供Object的类型信息，但是不提供动态创建，动态调用函数等功能，请问还有没有意义？这里要表达的意思主要是**类型系统的重要性**。



类型信息主要由虚幻引擎的元数据系统提供；动态创建和动态调用函数则分别由虚幻引擎的不同子系统（如 FReflectionRegistry 和反射系统）提供。即便不提供动态创建和动态调用函数的功能，类型信息本身仍然具有极高的价值，因为它为整个系统的静态信息提供了坚实的基础



> 在这里我有一个问题出现了：动态创建和反射有什么联系呢？没有反射不是也可以在代码中动态创建吗？



### C++ RTTI

谈及C++运行时**类型系统**的时候，通常说Run-Time Type Inditification，这里只提供两种最基本的操作符：

#### typeid:

​	这个关键字作用是让用户知道是什么类型，并提供一些基本对比和name方法，作用的话呢。。。顶多就是让用户判断从属于不同的类型。用的不广泛



####  dynamic_cast

​	该转换符用于将一个**指向派生类对象**的**基类指针或引用**转换为派生类指针或者引用，使用条件是智只能用于含有虚函数的类。转引用失败会返回bad_cast,转换指针失败会返回null

​	dynamic_cast内部机制其实是利用虚函数表中类型信息判断基类指针是否指向派生类对象。其目的更多是用于运行时判断对象指针是否为一个特定的子类的对象。

​	其他的运用吗模板，和宏标记就是编译时的手段了。



#### C++当前实现反射的手段

​	以下几种流派：

- 宏

- 模板

​	举一个Github实现比较优雅的C++RTTI反射库做例子：[rttr](https://link.zhihu.com/?target=https%3A//github.com/rttrorg/rttr)

- 编辑器数据分析

- 工具生成代码，最好的就是Qt中的反射：

​	Qt中的反射过程是基于moc实现，用一个**元对象编译器**在程序编译前，分析C++源文件，识别一些特殊的宏，然后生成对应的moc文件，之后再一起全部编译链接。



​	

> 说实话，举得这些例子这些反射我也看不懂，也不想看，直接快进到UE中的反射吧



#### UE中的UHT方案

​	和Qt差不多，实现在C++源文件中用**空的宏**做标记，然后用UHT分析生成的generated.h和generated.cpp文件，之后再一起编译

```c++
UClASS()

class Hello_API UMyClass :public  UObject
{
	GENERATED_BODY()
public:

UPROPERTY(BlueprintReadWrite,Category="Test") 括号中的是元数据，用于指定属性的访问权限和分类
float Score;

UFUNCTION(BluprintCallable,Category="Test")
void CallableFunctionTest();

UFUNCTION(BLUEPRINTNATIVEEVRNT,Category="Test")
void NativeFuncTest();

UFUNCTION(blueprintimpleementEvent,Catrgory="Test")
void ImplementableFunctionTest();


}



啥是空标记呢？
    UCLASS() GENEATED_BODY() UPROPRERTY UFUNCTION都是空的宏。
    空的宏本身不包含任何代码逻辑，只是一个标记，用于告诉UHT对其进行处理
```

**这种方法的优点：**

对C++代码修改比较小，不会破坏C++的类声明结构，开发者只需要在代码中添加这些宏便能方便的将元数据与代码关联起来，而具体的实现细节会由UHT生成并隐藏起来。





#### 总结

​		本章主要解释了，为啥要以类型系统作为搭建Object系统的第一步 ，然后描述了C#的反射系统长啥样，接着说C++的RTTI哦工具，还有QT的宏标记反射实现机制，看看别人手里有什么，看看自己手里有什么。想想为啥UE选择这种方案？既然选择这种方案当然是优点的，就是对C++修改小，不破坏类声明结构，只需要在源码中添加一些空宏告诉UHT处理，然后就能实现将元数据和代码关联起来。



# UObject(三)类型系统设定与结构



### 引言：

​	上篇中的结论是UE采用UHT的方式搜索并生成反射所需的代码。接下来开始着手构建设计真正的类型系统。

上篇中给我看蒙蔽的地方是 他说了一句：

> 虽然上面说了很多反射的术语描述我们熟知的运行时的类型信息的系统，动态创建类对象等，但是其实“反射”是在“类型系统”之后实现的附加功能，不要只看反射而忘记朴素的本质支撑。

​	动态创建对象这里，我看的有点不懂了，这里和反射的区别，以及和C++中通过new动态创建对象有什么区别。

​	梳理出来的思路：动态创建对象是需要知道创建出来的对象的类型的，而反射是可以在游戏中动态获取所需要对象的类型的。所以这里反射提供了很大的便利，大钊的意思是如果没有反射只用类型系统中信息的话也是可以实现的，GC也是可以实现的，只是会有些不方便。

​	**没有反射的时候**动态创建需要在编译时预先知道所有可能的对象类型，并编写相应的工厂代码。没有反射要动态调用函数的话需要其他机制（如回调函数、委托等）来替代反射提供的动态方法调用能力。



​	既然类型系统这么牛逼，那么就开始学习他的架构吧！

### 前置设定

​	我们不可避免的会谈到UHT(Unreal Head Tool，一个分析源码标记并生成代码的工具)。

​	假设我们已经接受了class ,变量等前面加上空宏的设定。这里有两点是需要注意的。

1. 我们可以通过给类，函数，枚举，属性等特定宏来标记更多的元数据
2. 有必要时，甚至可以将其安插进生成的代码来合成编译

暂且不管UHT的工作方式，只需知道有这样一个工具每次编译前扫描代码，获取宏的位置和内容，然后分析下一行代码的声明含义。最后生成所需的代码



接着问题又来了：

- ​	为撒生成的是代码不是数据文件呢？

​	避免不一致性。将反射数据生成为C++代码好处之一就是保证其与二进制文件同步，永远不会加载陈旧的反射数据。【看不懂。。】

- 如果标记应该分析那个文件？

​	既然生成的时C++文件，假如有个类A,生成了A.generated.h和A.generated.cpp。 此时A.h是需要include "A.generated.h"的（联合起来）。 而且A.generated.h要放在其他include的最后，防止因为宏定义顺序产生问题。



### 结构

接受设定之后，就简单了，来的简单的HelloWorld.h文件

```C
#include "HelloWorld.generated.h"
UClass()
class HelloWorld(){
    UPROPERTY()
        int HELLO;
    UFUNCTION()
        void print();
}
```



UHT后面讲，所以先假设UHT为我搜索了所有信息，信息存在哪里呢？不知道，但是先看一下存储结构： ![img](https://picx.zhimg.com/v2-7cf1e2a1a4a7d2f9b513001adfc41711_1440w.jpg)

C++有声明和定义是知道的，黄色的都是**声明**，绿色(UProperty)的是**定义**。

然后他说把类型分为可聚合其他成员的类型和“原子”类型。

##### 聚合类型：

- UFUNCTION :属性可作为函数的输入输出参数
- USCRIPTSTRUCT：只可包含属性，在UE中，可以看作轻量的UObject,拥有和UObject一样的反射支持、序列化、复制等，但是其不受GC控制，需要自己控制内存分配和释放。
- UCLASS ：可包含属性和函数，是我们平时接触最多的类型

##### 原子类型：

- UENUM：支持普通枚举和enum class 
- int ,FString  基础类型啊

聚合类统一起来就成了Ustruct基类，可以把一些通用的添加属性等方法放进去，同时可实现继承。这个类型是其他三种的父亲。。这里的UStruct注意下，和在C++中编码时使用的可不一样，在C++编码中当**结构体**用的话其实是**UScriptStruct**。

```c++
USTRUCT（BlueprintType)
struct MyStruct{
GENERATED_BODY()

UPROPERTY(EditAnywhere,BlueprintReadWrite)
int32 MyInt;

UPROPERTY(EditAnywhere,BlueprintWrite)
FString MyString; 

};
幕后引擎会为FMyStruct 生成一个对应的USriptStruct对象
```

还有种类型比较特殊，接口，可以继承多个接口。和C++中虚类一样，但是**UE中的接口只可以包含函数**。一般来说自己定义的普通类要继承于UObject,但是如果要想把这个类当作接口就要继承自UInterface，但是生成的数据依旧用UClass存储

好的，声明说完了，现在说图中被标记为绿色的**定义**:UProperty,其可以理解为用一个类型定义个字段“type instance” 【这说的什么玩意儿~_~！】。UPPROPERTY有很多子类，比如UBoolProperty,UMapProperty,UDelegatePrtoperty....【但是好像用的也不多啊】



元数据UMetaData 其实就是TMmap<FName,FString>的键值对，用于为编辑器提供分类，友好名字，提示等信息，最终发布的时候不会包含此信息，【就是鼠标悬停在编辑器上的时候一些提示或者是名字啥的，仅限在编辑器中看到】



除了interface，这几个声明定义都是UField的派生类。

#### 那为什么还要有基类UField呢？

1. 抽象出来一层，**统一所有的类型数据**，如果所有类型数据类都有个基类，那么很容易用一个数组把所有的类型数据引用起来，方便遍历。另一个就是顺序问题，比如源文件中类型A中定义了P1，F1，P2，F2这样属性和函数交叉定义，经过UHT处理之后也可以是同样的数据，回溯出来定义的话，也是和原始代码顺序一致的。如果属性函数分开保存就会麻烦一些
2. 不管是声明还是定义（UPROPERTY，UFUNCTION,USCRIPTSTRUCT,STRUCT）都可以附加一份元数据UMetaData.所以应该有个基类中共保存UMetaData
3. 方便添加一些额外方法作为虚函数，供几个子类重载实现。

Ufield 字段，领域 顾名思义 ，不管是声明还是定义，都可以看作类型系统中的一个字段。

> 看半天类型数据，类型系统，说的什么玩意儿？ 就是以上这几个东西，经过UHT处理之后生成的类型，为什么我会在这里特殊说一下类型数据这个名词，因为接下来的问题答案中有一个“也”字



> 同时再明确下类型数据和类型系统的描述：虚幻引擎的类型系统是一个复杂且层次分明的体系，其核心目标是对各类数据和功能进行精确描述、管理以及提供运行时操作的能力。类型数据则是类型系统的具体体现，包含了各种类型的详细信息，比如类、结构体、枚举等的元数据。

#### 为什么UField要继承于UObject?

​	这个问题其实是在问，为什么类型数据**也**要同样继承于UObject? 【这里我突然意识到这些类型数据是和Actor，controller这些类性质是不一样的，这些属于一些“数据”】

然后要回答这个问题，其实是要看下UObject上哪些功能是所需的：

- GC:可有可无，因为这些类型数据一开始分配了就不可以释放了。当前GC系统是用类型系统来支持对象引用遍历
- 反射：略
- 编辑器集成：也可以没有，编辑器就是利用类型数据来进行集成编辑的，当然，当我们在蓝图中创建函数、变量等操作其实也可以看作是在编辑类型数据。【顿悟，蓝图中创建的函数、变量不就是C++经过处理之后的元数据嘛？不，是相似，不能说是相同。**蓝图**的元数据以一种特定的内部数据结构存储在蓝图文件（`.uasset`）中。**C++** 代码经过 UHT 处理后，元数据会以 C++ 类和结构体的形式存在于生成的代码文件中，并且与原始的 C++ 代码紧密关联】
- CDO,不需要，每个类型的类型数据一般只有一份，CDO是在对象身上的
- 序列化。必须有，类型数据要保存下来。比如蓝图创建的类型
- Replicate。用处不大
- RPC ,wusuowei1
- 自动属性更新，也无所谓，类型数据一般不会频繁变动。
- 统计，可有可无

​	总结发现，序列化是最重要的功能，GC和其他功能属于锦上添花。所以本着统一思想，所有类型数据都继承自UObject了，这样写序列化的操作不用写两套了。

​	在对象身上，可以用Instance->GetClass()来获取UClass对象，在UClass本身调用GetClass()返回的是自己本身，这样可以区分对象和类型数据。

![img](https://pic2.zhimg.com/v2-59142324b676346608c872022097d5c3_1440w.jpg)

### 总结

从UE3修修补补过来的，挺不错了，但是有些地方需要注意：接口数据类型依旧是UClass 但是不允许包含属性，这个在结构上并未做限制，只能通过UHT检查和代码中类型判断区分；比如UStruct中包含的是UField链表，其实隐含意思就是可以有属性也可以有嵌套类型。UMetaData虽然在UPackage用的TMap<UObject*,TMap<FName,FString> >来映射，但是实际上只有UField中有GetMetaData接口，一般只有Field跟其关联。

​	当然我们关注的是如何理解UE这套类型系统（也叫属性系统）【类型系统就类型系统不得了】。



再补充一下，为甚说UProperty属于定义？...AI给的答案有点智障，，，

还有看评论虚幻五已经架构和这个有些不同了，这个大致了解下就行。

# UObject(四) ---这是给我干哪儿来了？ 

> 大致浏览了一下这一篇，就两个字，懵逼，这是给我干哪儿来了，这还是国内吗？

### 引言

​	首先明确下，我们分析UHT生成后的代码，这是属于那一部分了。UE的类型系统构建主要两个大阶段-- 宏标记阶段、后续构建阶段。宏标记阶段以及类型的结构我们在之前也讨论过了，那么这里就到后续的构建阶段



构建阶段又分为 生成、收集、注册、链接等这些阶段。‘

本片就是讲了很多生成阶段的文件分析，让我更了解生成的generated.cpp文件是什么样的



Note:首先是生成代码的注册过程都会因为HotLoad 这个功能开启有些不同，所以在这里要首先关闭hotload。在build.cs中加上一行 Definitioin.Add("WITH_HOT_RELOAD_CTORS=0");



> 我从后面回来的，这个懒加载初始化挺重要的，之前没明白大钊写在这里的目的、、

### C++ Static Lazy

 初始化模式，两种，，一种返回指针、一种返回引用

```c++
Hello* StaticGetHelo(){

static Hello* obj =nullptr;
if(!obj){
	obj=..
}
return obj ;
}
或者是
Hello& StaticGetHello(){

    static Hello obj(...);
    return obj;
}
```

使用引用跟简洁安全  。前者使用指针的话，单线程情况下是够用的。为啥用指针呢？因为有些情况对象生命周期是别的地方管理的，如UE的GC，因此这里只static化一个指针

### UHT代码生成

​		在C++中的预处理就是对源代码进行宏定义展开、头文件包含、条件编译、注释删除等操作。 同样当我们使用了宏标记的时候，我们都需要进行简单或者复杂的词法分析，提取出有用的信息，然后生成所需的代码。

擎里创建一个空C++项目命名为Hello，然后创建个不继承的MyClass类。**编译之后**，UHT就会为我们生成以下4个文件：

- HelloClasses.h：目前无用
- MyClass.generated.h：MyClass的生成头文件
- Hello.generated.dep.h：Hello.generated.cpp的依赖头文件，也就是顺序包含上述的MyClass.h而已
- Hello.generated.cpp：该项目的实现编译单元。



UHT生成的代码文件中函数有很多是Z_ 开头的，并没有什么特殊含义，只是为了避免命名冲突。



### UClass生成代码剖析

```C++
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"  就是引用了一些更基础的头文件，比如vector.h等，因此在此文件中就不用再引入了。
#include "MyClass.generated.h" 引用生成的头文件

UCLASS()
class HELLO_API UMyClass : public UObject
{
	GENERATED_BODY()
	该宏是重中之重，其他UClass 宏只是提供信息，不参与编译，但是GENERATED_BODY正是将生命和元数据关联在一起的枢纽  ！！这个GENERATED_BODY很重要
	
};
```



继续看宏代码。两个绑定宏，一个GENERATED_BODY宏

```
#define BODY_MACRO_COMBINE_INNER(A,B,C,D) A##B##C##D
#define BODY_MACRO_COMBINE(A,B,C,D) BODY_MACRO_COMBINE_INNER(A,B,C,D)
#define GENERATED_BODY(...) BODY_MACRO_COMBINE(CURRENT_FILE_ID,_,__LINE__,_GENERATED_BODY)
```

CURRENT_FILE_ID  ：就是整出了一个ID名字

`__LINE __   `: 就是这个宏在使用时候的行数，你可以看看源C++代码是11行，这里就是11 ，所以有了这个就支持在**同一文件中声明很多类**，其他类被GENERATED()标记之后，预处理阶段处理之后就是xxx_xxx_h_行号 _GENERATED_BODY,这样的名字

_GENERATED_BODY 加上的后缀



最后得到了==》Hello_Source_Hello_MyClass_h_11_GENERATED_BODY。【啊？这就combine起来了？】

> 所以GENERATED_BODY宏其实是生成另外一个宏的名称，就上面这一长串

### MyClass.generated.h

UHT分析生成的文件内容如下：

https://zhuanlan.zhihu.com/p/25098685大钊文件中看吧+-+!

看这部分的话，初学者对宏不了解的，看着这么多大写字母真的无从下手，首先预览下，可以看到不同名字的结构从上到下大致分以下这样的大致结构，具体内容可以先不看，先梳理框架：

```
#define Hello_Source_Hello_MyClass_h_11_INCLASS_NO_PURE_DECLS \

#define Hello_Source_Hello_MyClass_h_11_INCLASS \

#define Hello_Source_Hello_MyClass_h_11_STANDARD_CONSTRUCTORS \

#define Hello_Source_Hello_MyClass_h_11_ENHANCED_CONSTRUCTORS \


接着就是两个比较重要的定义
#define Hello_Source_Hello_MyClass_h_11_GENERATED_BODY_LEGACY \ //两个重要的定义

#define Hello_Source_Hello_MyClass_h_11_GENERATED_BODY \    //两个重要的定义

```



整理号顺序之后，就可以从下向上看整个文件的内容，首先能够看到两个上文说过的GENERATED_BODY定义，这两个宏都包含了刚才我们梳理结构出来的其他宏，比如xxxx_INCLASS、xxxx_CLASS_NO_PURE_DECLS  (xxxx是简写 大家应该能看的出来，这里只需要区分这两宏名称)，可以看到这俩个宏定义一模一样。xxxx_STANDARD_CONSTRUCTORS和xxxx_ENHANCE_CONSTRUCTORS他俩几乎也一样，只是差了一个Super(ObjectInitializer){}；构造函数的默认实现



#### 先看这个叫增强构造的

xxxx_ENHANCE_CONSTRUCTORS

```c++

#define Hello_Source_Hello_MyClass_h_11_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMyClass(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used 禁止掉C++11的拷贝构造*/ \
	NO_API UMyClass(UMyClass&&); \
	NO_API UMyClass(const UMyClass&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMyClass); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMyClass); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMyClass) 接下来说这个

```

接着： DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL

```c++
#define DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(TClass) \
	static void __DefaultConstructor(const FObjectInitializer& X) { new((EInternal*)X.GetObj())TClass(X); }
```

声明定义了一个**构造函数包装器**。因为根据名字反射对象的时候，需要调用该类的构造函数，而类的构造函数并不能用函数指针指向，所以这里用**static函数** 包装一下使其变成一个“平凡的”函数指针，而且所有**类的签名**一致，就可以在UClass里用一个函数指针里保存起来

【说的啥呢，就是要调用构造函数的的时候，**类的构造函数并不能用函数指针指向**，所以包装下，能调用构造函数就好了】

然后想在实现反射框架的时候也可以用更加便捷的方式，使用模板：

```c++
template<class TClass>
void MyConstructor(const DObjectInitializer& x){
new ((EInternal*)X.GetObj())TClass(X);
}
```

#### 再接着看xxx_INCLASS：

```c++
#define Hello_Source_Hello_MyClass_h_11_INCLASS \
	private: \
	static void StaticRegisterNativesUMyClass(); \   义在cpp中，目前都是空实现
	friend HELLO_API class UClass* Z_Construct_UClass_UMyClass(); \  个构造该类UClass对象的辅助函数
	public: \
	DECLARE_CLASS(UMyClass, UObject, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/Hello"), NO_API) \   声明该类的一些通用基本函数
	DECLARE_SERIALIZER(UMyClass) \  声明序列化函数
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};   这个标记指定了该类是C++Native类，不能动态再改变，跟蓝图里构造的动态··行区分。
```

> 大钊说  DECLARE_CLASS  是最重要的声明了
>

`	DECLARE_CLASS(UMyClass, UObject, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/Hello"),	NO_API) \   //声明该类的一些通用基本函数`

- 第二个参数为**TClass** ：基类名字、
- 第三个参数**TStaticFlags**:类的属性标记 0，代表默认 
- 第四个参数**TStaticCastFlags**: 指定该类可以转换成那种类，0代表不能转换为默认类【具体不能转成什么默认类，可以在EClassCastFlags声明中看】
- 第五个参数**TPackage**：类所处的包名，所有对象都必须在一个包中，这里是"/Script/Hello"，指定是Script下的Hello，Script可以理解为用户自己的实现，不管是C++还是蓝图，都可以看作是引擎外的一种脚本。Hello就是项目名字。
- 第六个参数参数**TRequiredAPI**:就是用来DLL导入导出的标记，这里是NO_API,不需要导出。

```c++
#define DECLARE_CLASS( TClass, TSuperClass, TStaticFlags, TStaticCastFlags, TPackage, TRequiredAPI  ) \
private: \
    TClass& operator=(TClass&&);   \
    TClass& operator=(const TClass&);   \
	TRequiredAPI static UClass* GetPrivateStaticClass(const TCHAR* Package); \
public: \
	/** Bitwise union of #EClassFlags pertaining to this class.*/ \
	enum {StaticClassFlags=TStaticFlags}; \
	/** Typedef for the base class ({{ typedef-type }}) */ \
	typedef TSuperClass Super;\
	/** Typedef for {{ typedef-type }}. */ \
	typedef TClass ThisClass;\
	/** Returns a UClass object representing this class at runtime 运行时返回一个表示该类的 UClass 对象 */ \
	inline static UClass* StaticClass() \
	{ \
		return GetPrivateStaticClass(TPackage); \
	} \
	/**返回此类的静态类标志 */ \
	inline static EClassCastFlags StaticClassCastFlags() \
	{ \
		return TStaticCastFlags; \
	} \
	DEPRECATED(4.7, "operator new has been deprecated for UObjects - please use NewObject or NewNamedObject instead") \
	inline void* operator new( const size_t InSize, UObject* InOuter=(UObject*)GetTransientPackage(), FName InName=NAME_None, EObjectFlags InSetFlags=RF_NoFlags ) \
	{ \
		return StaticAllocateObject( StaticClass(), InOuter, InName, InSetFlags ); \
	} \
	/** For internal use only; use StaticConstructObject() to create new objects.  */ 
        仅供内部使用；使用StaticConstructObject（）创建新对象。\
	inline void* operator new(const size_t InSize, EInternal InInternalOnly, UObject* InOuter = (UObject*)GetTransientPackage(), FName InName = NAME_None, EObjectFlags InSetFlags = RF_NoFlags) \
	{ \
		return StaticAllocateObject(StaticClass(), InOuter, InName, InSetFlags); \
} \
	/** For internal use only; use StaticConstructObject() to create new objects. */ \
	inline void* operator new( const size_t InSize, EInternal* InMem ) \
	{ \
		return (void*)InMem; \
	}  

external 外部 interal 内部
```

其中的`StaticClass`就是我们经常用到的函数，内部调用了GetPrivateStaticClass其实现正是在Hello.generated.cpp中里的. 下面的内容就能看到这个函数的实现。



### HELLO.generated.cpp

整个HELLO生成一个这个文件，这个文件都有什么功能呢？

有以下几个重要的点：

1. IMPLEMENT_CLASS(UMyClass, 899540749);  这个是跟上面MyClass.generated.h文件中的DECLARE_INCLASS对应的
2. #if USE_COMPILED_IN_NATIVES  宏编译的时候打开的逻辑，其中包含一些逻辑（注册，链接等
3. 构造Hello的UPackage



#### 1中的IMPLEMENT_CLASS; 

```c++
#define IMPLEMENT_CLASS(TClass, TClassCrc) \
	static TClassCompiledInDefer<TClass> AutoInitialize##TClass(TEXT(#TClass), sizeof(TClass), TClassCrc); \   //延迟注册
	UClass* TClass::GetPrivateStaticClass(const TCHAR* Package) \   //.h里声明的实现，StaticClas()内部就是调用该函数
	{ \
		static UClass* PrivateStaticClass = NULL; \      又一次static lazy
		if (!PrivateStaticClass) \
		{ \
			/* 这可以用模板来处理，但我们希望它在外部以避免代码膨胀 */ \
			GetPrivateStaticClassBody( \    //该函数就是真正创建UClass*,以后
				Package, \  //Package名字
				(TCHAR*)TEXT(#TClass) + 1 + ((StaticClassFlags & CLASS_Deprecated) ? 11 : 0), \//类名，+1去掉U、A、F前缀，+11去掉_Deprecated前缀
				PrivateStaticClass, \   //输出引用
				StaticRegisterNatives##TClass, \
				sizeof(TClass), \
				TClass::StaticClassFlags, \
				TClass::StaticClassCastFlags(), \
				TClass::StaticConfigName(), \
				(UClass::ClassConstructorType)InternalConstructor<TClass>, \
				(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>, \
				&TClass::AddReferencedObjects, \
				&TClass::Super::StaticClass, \
				&TClass::WithinClass::StaticClass \
			); \
		} \
		return PrivateStaticClass; \
	}
```



经过处理之后人肉展开的效果可以看到，展开之后就清晰很多，这样.h的声明和.cpp的定义就全都有了。不管定义了多少函数，要记得注册的入口就是那两个static对象在程序启动的时候登记信息，才有了之后的注册。





> 

反射是一种在运行时检查、访问和修改程序的类型信息、成员变量、成员函数等的能力。在传统的静态类型语言（如 C++）里，类型信息在编译时就已经确定，运行时难以动态获取和操作。不过，借助反射机制，程序能够在运行时做到以下几点：

- **类型信息查询**：能够在运行时获取某个对象的类型信息，像类名、父类、成员变量、成员函数等。
- **成员访问**：可以在运行时动态访问和修改对象的成员变量，调用成员函数。
- **动态创建对象**：能够在运行时依据类型信息动态创建对象。
