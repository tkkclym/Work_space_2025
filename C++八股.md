#  new与delete和malloc与free的区别：

1. new自动计算分配空间的大小，malloc需要手动计算

2. new返回的是具体对象类型，malloc返回的是void* 还需要类型转换

`Task *ptask=(Task *)malloc(sizeof( *ptask) );`

3. 分配失败后的不同：new分配失败会抛出**异常**，malloc分配失败会返回**null**

4. new 分配内存之后会初始化成员变量，而且建立虚拟内存和物理内存的映射关系,    malloc分配的是虚拟内存，但是因为没有进行初始化，在页码中只有虚拟内存，而要映射到物理内存需要缺页中断找到其映射关系。所以没有创建与物理内存之间之间的页表

5. malloc 在堆上分配，

   对于小于128k的内存分配，他会使用**brk系统**从其维护的内存池中分配内存，而不是每次都进行系统调用，这样可以提高效率。

   

   如果超过128k的话，就是在文件映射区进行分配的，对于较大的内存分配，`malloc`可能会使用`mmap`系统调用来直接在进程的虚拟地址空间中分配内存。`mmap`可以映射内存页到文件或匿名内存，对于大内存块的分配更为高效。

**6.调用new的时候，会发生什么？**

​			首先会调用operator new ，

​			然后申请足够的内存空间，

​			最后再调用其构造函数初始化成员变量。

1.delete 释放内存 需要对象类型的指针，free只需要void*类型的指针

**2.delete操作时发生什么？**

​		1.将会调用析构函数  

​		2.再调用operator delete       `operator delete`调用底层系统调用（如`malloc`的对应`free`）

​		3.最后释放空间 

调用析构函数的时候：如果是单个对象，会调用对象的析构函数执行清理工作。如果是对象数组，按照从后向前的顺序调用每一个元素的析构函数。



**3.free调用时只用调用void * ，他如何知道释放多少空间呢？**

​			malloc分配空间时多给**16字节**空间，其存储内存块的描述信息，包含了长度信息，所以调用void*指针，他可以知道会释放多大空间

4 .free释放内存之后，内存还在吗？

​				1.如果是brk调用的内存，还是要回到内存池中的。

​				2。如果是mmap调用的将会立刻释放物理内存	

**与C++构造函数和析构函数的集成**：

- `new`在分配内存后会调用对象的构造函数，`delete`在释放内存前会调用对象的析构函数。
- `malloc`和`free`不与C++的构造函数和析构函数集成，因此在使用它们分配和释放对象时，**需要手动调用构造函数和析构函数。**



### delete时候，会不是释放指针占用的资源？

不会，比如 AA *p=new AA("ccc");delete p； 这个指针p的内存是在栈上分配的，delete的时候释放的是p指向的堆内存和AA占用的资源，当`p`的作用域结束时（比如函数结束），`p`所占用的栈内存会自动被释放。

# 析构函数：

为什么会出现先调用了子类析构函数再调用基类析构函数的现象？

首先，因为派生类是从基类派生得来的，所以在派生类构造的时候，首先会构造继承来的东西，就是父类教给他的一些属性和方法。然后再构造他自己的属性和方法。

析构的时候因为他有自己的学来的东西和父亲传承给他的东西，所以析构的时候，先调用自己的析构函数，把他们清理完，然后再调用父类的析构函数将其清理完。

当时还在迷惑为什么不是只调用了子类的析构函数，为什么还要多调用父类的析构函数。

原因如下：当派生类继承自基类时，它可能继承了基类分配的资源。基类的析构函数负责释放这些资源。如果仅调用派生类的析构函数而不调用基类的析构函数，那么基类分配的资源将不会被释放，从而导致资源泄漏。

![image-20240908203919967](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240908203919967.png)

### 析构函数可以是纯虚函数：

在C++中，将析构函数声明为纯虚函数意味着该基类算是一个抽象类，不能直接实例化，这样做通常是为了派生类必须提供自己的析构函数实现



析构函数的作用不是删除对象！！是为了清理对象占用的资源，也就是清理内存，使这部分内存可以被程序重新分配给新对象使用。

### 析构函数释放哪些资源？

析构函数负责清理对象在生命周期内分配的资源

1. 分配的动态内存，在构造时动态分配的内存应在析构函数中释放：

```c++
class MyClass{
public:
	MyClass(){
	ptr=new int [10];
	}
	~MyClass(){
	delete []ptr;
	}
	private:
	int *ptr;
}
```

2. 打开的文件句柄，如果类中有打开文件的成员，应该在析构函数中关闭这些文件：

```c++
class Filehandle{
public:
    Filehandle(const char* filename){
        file=fopen(filename,"r");
    }
    ~Filehandle(){
        if(filr){
            fclose(file);//关闭文件
        }
    }
    private:
    FILE *file;
}
```



3. 资源句柄和外部对象  对于操作系统资源，如套接字、文件描述、数据库连接等，需要在析构函数中进行适当关闭或释放操作。

```c++
class NetworkSocket {
public:
    NetworkSocket() {
        // 创建套接字
    }
    ~NetworkSocket() {
        // 关闭套接字
    }
};

```



delete 删除对象的时候首先会调用析构函数，然后再调用operate delete，最后就是释放资源



# 进程和线程的区别：

## 本质区别：

进程：资源分配的基本单位

线程：CPU调度的基本单位

进程中有多个线程

### 并发性

就是CPU一直切换，一段时间运行一个进程，几个进程切换进行

并行就是一个CPU执行一个进程，几个CPU同时执行

**上下文切换：**切换的时候保存现场信息 以便现场环境的恢复

现场环境包括 CPU，寄存器，程序计数器，用户空间信息（进程占有虚拟内存）

同一进程之间的线程切换，不需要切换太多信息（ CPU，寄存器，程序计数器）。不涉及资源切换，切换效率高。

而不同进程中不同线程的切换，则需要切换很多信息，涉及资源切换。切换效率低。

### 内存

进程拥有独立虚拟地址空间，线程通常 共享一些进程中的空间，通常只分配一些栈，PC,本地存储等8兆左右的独立空间

​	

#### 进程的使用有什么好处？

因为有独立的虚拟内存空间，进程之间是相互隔离的，所以当我们一个软件使用多进程的时候，不会因为其中一个进程失效导致软件崩溃，而且还会有一个进程管理去重启一个进程



多线程的话，因为线程共享一个进程的虚拟地址空间，但是多线程需要加锁（涉及临界资源）

### 健壮性

进程健壮性高，因为一个进程崩溃，不会影响其他进程

而线程健壮性就比较低，因为线程崩溃的话，就会导致他所属的这个进程崩溃













# 描述系统调用的整个流程









# TCP：

“三次握手”的目的是“为了防止已失效的连接(connect)请求报文段传送到了服务端，因而产生错误”，也即为了解决“网络中存在延迟的重复分组”问题。例如：Client发出的第一个连接请求报文段并没有丢失，而是在某个网络结点长时间的滞留了，以致延误到连接释放以后的某个时间才到达Server。本来这是一个早已失效的报文段。但Server收到此失效的连接请求报文段后，就误认为是Client发出的一个新的连接请求。于是就向Client发出确认报文段，同意建立连接。假设不采用“三次握手”，那么只要Server发出确认，新的连接就建立了。由于现在client并没有发出建立连接的请求，因此不会理睬server的确认，也不会向Server发送数据。但Server却以为新的运输连接已经建立，并一直等待client发来数据。这样，Server的很多资源就白白浪费掉了。采用“三次握手”的办法可以防止上述现象发生，Client不会向Server的确认发出确认。Server由于收不到确认，就知道client并没有要求建立连接。



## TCP和UDP区别：

TCP（传输控制协议）：面向字节流，进行传输，提供可靠服务，适用于文件传输，邮件传输，信息传输等需要可靠传输的操作。通信双方需要三次握手才能建立TCP连接进行通信。通信完成还要四次挥手断开连接。并且TCP不支持多播和广播

UDP（用户数据报协议)：无连接，面向数据报文段进行传输，提供非可靠服务，传输速度比TCP快，占用资源比TCP少，首部8字节，仅仅提供校验和来保证报文完整性。消炎势必直接丢弃不做任何处理。

## 三次握手

为什么TCP需要三次握手才能建立连接：

1.第一次握手是客户端向服务端发送带有SYN标志的数据包，服务端接收到数据包之后，服务端得知客户端发送能力正常，同时服务端得知自己接收能力正常。

2.第二次握手，服务端向客户端发送带有SYN/ACK标志的数据包，客户端收到数据包后，客户端得知服务端的接收和发送能力正常，客户端的发送，接受能力正常。

3.第三次握手，客户端向服务端发送带有ACK标志的数据包，服务端收到这个数据包后，服务端知道了客户端接收能力正常，自己的发送能力正常	

**三次握手结束之后，Client和Server都互相得知对方和自己的发送和接收能力正常。**

缺少任何一步，都不能使双方成功相互确认收发功能。

**为什么不是两次握手呢？为什么客户端最后还要确认一次？**

假如是两次握手就能建立连接，那么假如Client发送了一个有SYN标志的请求信息，但是在网络中阻塞了很久没有到达Server端，于是Client又发送了一次SYN标志的请求，那么这次Server收到了，两次握手，建立了连接，传输数据，关闭连接，那么等结束之后，此时第一个SYN请求又来了，Server收到了，会再次连上，这个已经不再需要的连接，不仅浪费资源，还可能造成错误。



##  再来说四次挥手

<img src="https://i-blog.csdnimg.cn/blog_migrate/8edbc1d65196a40d5bd0f4fcc36e9370.png#pic_center" alt="在这里插入图片描述" style="zoom:67%;" />



挥手之前C和S是互相传输数据的

1. 第一次挥手，是客户端向服务端发送FIN=1标志的数据包，然后客户端进入FIN-WAIT-1阶段，客户端不再向服务端发送数据，但是依然可以接受数据。
2. 第二次挥手，Server收到Client发来的FIN标志的关闭连接报文，Server通知上层应用后向Client发送ACK标志的确认报文，然后进入CLOSE-WAIT阶段。                                                                                                                                                                            记住客户端此时仍然接收Server的报文，他收到ACK报文后，进入FIN-WAIT-2阶段，继续接受Server的报文，并且等待Server的FIN标志的关闭报文。
3. 第三次挥手，服务端传输完最后的数据，向Client发送FIN关闭标志的报文，并进入LAST-ACK阶段，等待客户端的最后确认。
4. 第四次挥手，Client收到Server的FIN关闭报文之后，发出最后的ACK确认报文，进入TIME-WAIT阶段，这个阶段会等待2MSL（最大报文生存时间）后释放TCB（传输控制块）进入CLOSED状态才是真正的关闭连接。

​	而Server端收到Client的ACK标志的确认报文之后，立刻释放Server的TCB进入CLOSED状态完成关闭连接。

哈哈哈哈哈服务器收到ACK直接关了，Server关闭TCP连接的时间要比Client早

### 那为什么Client完成确认之后不立即释放资源，而是要等待2MSL？

等待2MSL是为了确保Client发送的ACK标志的确认报文可以正确到达Server，如果这个ACK标志的确认报文无法正常到达Server，那么Server就无法正常CLOSED，Server会在定时器到期之后重新发送FIN标志的连接关闭报文，而Client在等待2MSL的时间段中可以收到Server重传的这个报文，从而重新发送ACK确认报文，并重启2MSL时间的计时器。2mSL时间内没有收到Server发来的FIN标志的连接关闭报文，那么Client可以认为Server正确收到了最后的ACK确认报文，并且已经CLOSED关闭了连接。接着2MSL时间到期后，Client从TIME-WAIT状态切换到CLOSED状态，释放所有资源。



### 客户端关闭连接的详细步骤：

1. **客户端发送最后一个ACK**：客户端在收到服务器（Server）的FIN+ACK报文后，会发送一个ACK报文作为响应。
2. **客户端进入TIME_WAIT状态**：发送完ACK报文后，客户端不会立即关闭连接，而是进入TIME_WAIT状态，并开始计时2MSL。
3. **等待2MSL时间**：在2MSL时间内，客户端会保持TIME_WAIT状态。如果在2MSL时间内客户端没有收到服务器重传的FIN报文，那么客户端可以认为服务器已经正确接收到了最后的ACK确认报文，并且已经关闭了连接。
4. **客户端关闭连接**：当2MSL时间到期后，客户端会从TIME_WAIT状态转换到CLOSED状态，这时客户端会关闭连接，释放所有相关资源。



## TCP拥塞控制算法

四种拥塞控制算法： 慢开始、拥塞避免、快重传、快恢复

一开始是慢开始阶段，然后到达门限值就改为拥塞控制算法，

如果遇到超时情况，当发生超时（即没有收到ACK），TCP会将ssthresh设置为当前拥塞窗口的一半（或者一些实现中设置为更小的值），然后拥塞窗口重置为1个MSS，重新开始慢开始过程。

如果发送方收到3个连续的ACK，**就知道接收方确实没有收到报文段**，因而应当立即进行重传（即“快重传”）这样就不会出现超时，发送方也不就会误以为出现了网络拥塞。

当发送方收到连续三个重复的确认时，由于发送方现在认为网络很可能没有发送拥塞，因此现在不执行慢开始算法，而是执行快恢复算法FR(Fast [Recovery](https://so.csdn.net/so/search?q=Recovery&spm=1001.2101.3001.7020))算法

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240814205545708.png" alt="image-20240814205545708" style="zoom:67%;" />

1. **慢开始（Slow Start）**：当TCP连接开始时，拥塞窗口（cwnd）从一个较小的值开始，通常是1个最大报文段（MSS）。每收到一个确认（ACK），拥塞窗口加倍，这个过程称为指数增长。
2. **拥塞避免（Congestion Avoidance）**：当拥塞窗口增长到一个称为慢开始门限（ssthresh）的值时，TCP切换到拥塞避免阶段。在这个阶段，每经过一个往返时间（RTT），拥塞窗口增加1个MSS，而不是加倍。
3. **快重传（Fast Retransmit）**：当发送方收到三个重复的ACK时，它会立即重传丢失的数据包，而不是等待超时。这个过程称为快重传。
4. **快恢复（Fast Recovery）**：在执行快重传之后，TCP不执行慢开始算法，而是执行快恢复算法。在快恢复中，ssthresh设置为当前拥塞窗口的一半，然后拥塞窗口设置为ssthresh加上3个MSS（因为已经收到了三个重复的ACK，相当于已经确认了三个MSS）。然后，发送方继续按拥塞避免的规则增加拥塞窗口。



计算机网络

- 在HTTP协议中，请求消息使用( post)方法用于通过Internet发送大量数据，没有长度的限制。

- 将一块数据通过TCP成功发送到对方，对方拿到的这块数据和发送方肯定一模一样 ：

  错！  对方拿到的数据会经过多层协议包装，不会和发送方一模一样；

- 三次握手只能由客户端发起

# 调用构造函数：

![image-20240813165900087](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240813165900087.png)

MyClass a[5]这里是在栈上创建的5个对象，new的是在堆上创建对象 



`MyClass *p = new MyClass[5];` 这样的声明是正确的，并且确实是在堆上生成了一个`MyClass`类型的对象数组。这个过程如下：

1. `new MyClass[5]`：在堆上分配了足够存放5个`MyClass`对象的空间，并且调用了`MyClass`的构造函数5次，每个对象一次，以初始化这些空间。
2. `MyClass *p`：在栈上创建了一个`MyClass`类型的指针`p`。
3. `p = new MyClass[5];`：将堆上分配的内存块的首地址赋值给栈上的指针`p`，这样`p`就指向了这个对象数组的首个元素。

因此，`p`指向的是堆上分配的第一个`MyClass`对象的地址，你可以通过`p`来访问这个数组中的任何一个对象，例如`p[0]`是第一个对象，`p[1]`是第二个对象，依此类推。





MyClass *b[6]是在声明指针数组，，他只是分配了指针空间，而不是对象空间，所以不会调用构造函数。

而指针的大小一般是根据操作系统不同而不同

​	在大多数现代操作系统上，指针的大小通常是固定的，例如：	

- 在32位系统上，指针的大小通常是4字节。
- 在64位系统上，指针的大小通常是8字节。



# main函数的参数：

main函数有三个参数，argc、argv和envp，它的标准写法如下：
 int main(int argc, char *argv[], char *envp[])
 **int argc**，表示存放了命令行参数的个数（argument count）。
 **char \*argv[]**，表示传入main函数的参数序列或指针（argument vector）。
 **char \*envp[]**，表示存放当前程序运行环境的参数（environment parameter）。 



<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240813175818965.png" alt="image-20240813175818965" style="zoom:50%;" />

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240814204534718.png" alt="image-20240814204534718" style="zoom:67%;" />

# 虚表与虚表指针：

### 虚表：

一个类中有虚函数就会有虚表，如果一个类继承一个有虚函数的类，这个派生类也有了自己的虚表

虚表是一个指针数组，其元素是每一个虚函数的指针，每个元素对应一个虚函数的函数指针，而非虚函数（普通函数）调用不需要虚表

虚表内的条目，即虚函数指针的赋值在编译器的编译阶段，在代码编译阶段 ，虚表就已经构造出来了

虚函数指针和虚表指针可不同哦



#### 虚表在哪里？

虚函数表通常位于对象实例内存布局的开始部分，并且是对象内存布局的一部分

#### 虚表中除了有虚函数指针还有什么？

类型信息统计：这个指针指向一个类型信息结构。通常用于RTTI（运行时类型识别）

### 虚表指针：

虚表是属于类的，一个类有一个虚表，而同一个类的所有对象使用一个虚表。
为了指向对象的虚表，每个对象都有一个虚表指针，来指向自己所使用的虚表。所以为了让每个对象都能指向自己指向的虚表的虚表指针，编译器在类中添加了一个指针，*_vptr，用来指向虚表。这样，当类的对象在创建时便拥有了这个指针，且这个指针的值会自动被设置为指向类的虚表。

### 动态绑定：

<img src="https://i-blog.csdnimg.cn/blog_migrate/3bd3b35ecefd13afa68fd01d4facb5b0.png" alt="这里写图片描述" style="zoom:33%;" />

```c++
int main() 
{
    B bObject;
    A *p = & bObject;
    p->vfunc1();
}
```

这个会发生什么？

我们知道，B对象bobject 中有虚表指针指向B的虚表，虽然p是基类的指针只能指向基类的部分，但是虚表指针亦属于基类部分，所以p可以访问到对象bObject的虚表指针。bObject的虚表指针指向类B的虚表，所以p可以访问到B vtbl

程序在执行p->vfunc1()时，会发现p是个指针，且调用的函数是虚函数，接下来便会进行以下的步骤。 
首先，根据虚表指针p->__vptr来访问对象bObject对应的虚表。虽然指针p是基类A*类型，但是*__vptr也是基类的一部分，所以可以通过p->__vptr可以访问到对象对应的虚表。 

//当将派生类地址赋给基类指针的时候，基类指针实际指向的是派生类对象内存的起始位置地址。由于_vptr位于对象的开始部分，基类指针也间接通过这个 _vptr指向派生类的虚表

然后，在虚表中查找所调用的函数对应的条目。由于虚表在编译阶段就可以构造出来了，所以可以根据所调用的函数定位到虚表中的对应条目。对于 p->vfunc1()的调用，B vtbl的第一项即是vfunc1对应的条目。 
最后，根据虚表中找到的函数指针，调用函数。从图3可以看到，B vtbl的第一项指向B::vfunc1()，所以 p->vfunc1()实质会调用B::vfunc1()函数。



### ai的动态绑定总结：

在C++中，基类指针可以指向派生类的对象，这是多态性的核心特征之一。以下解释为什么基类指针可以访问到派生类对象的虚表：

1. **虚函数和多态性**： 当基类中声明了一个虚函数时，该基类及其派生类都会拥有一个虚表（vtable），这是一个函数指针数组，每个虚函数在虚表中都有一个对应的函数指针。
2. **虚表的继承**： 派生类继承基类时，它也会继承基类的虚表，但派生类可以覆盖（override）基类中的虚函数。覆盖一个虚函数时，**派生类虚表中对应的函数指针会被更新，指向派生类中定义的新函数**。
3. **指针的存储结构**： 在C++中，一个对象在内存中的布局通常包含其所有非静态成员变量。对于含有虚函数的类，对象的内存布局还会包含一个指向其虚表的指针（称为vptr）。当基类指针指向派生类对象时，这个vptr仍然存在，并且指向派生类的虚表。
4. **多态的动态绑定**： 当通过基类指针调用虚函数时，C++运行时会使用指针所指向对象的vptr来查找虚表，并调用相应的函数。这个过程称为动态绑定或晚期绑定。即使指针的类型是基类，由于动态绑定的机制，调用的实际上是派生类对象的虚函数（如果派生类覆盖了该函数）。



以下是这一过程的步骤：

- 基类指针指向派生类对象时，它实际上指向的是派生类对象的内存空间。
- 由于派生类对象的内存布局包含vptr，且vptr指向派生类的虚表，基类指针可以通过这个vptr访问到派生类的虚表。
- 当基类指针调用一个虚函数时，运行时会通过vptr找到对应的虚表，然后调用虚表中相应的函数指针，最终执行的是派生类中覆盖后的函数。



​		可以看到，通过使用这些虚函数表，即使使用的是基类的指针来调用函数，也可以达到正确调用运行中实际对象的虚函数。 
​		我们把经过虚表调用虚函数的过程称为动态绑定，其表现出来的现象称为运行时多态。动态绑定区别于传统的函数调用，传统的函数调用我们称之为静态绑定，即函数的调用在编译阶段就可以确定下来了。

### 我的总结：

​			派生类对象继承的是基类的**虚函数**，并在此基础上**构建自己的虚表**。派生类对象生成的时候，内存中存在虚表指针，指向虚表，虚表中有虚函数，每个虚函数在虚表中都有一个对应的函数指针。

 虚表指针 `_vptr` 本身并不会更新。派生类对象在创建时，其 `_vptr` 会被初始化为指向派生类的虚表。这个虚表中包含了基类虚函数的指针（如果未被覆盖）和派生类覆盖后的虚函数指针。

为什么基类指针能调用派生类的虚函数：

- 基类指针能调用派生类的虚函数，是因为派生类继承了基类的虚函数，并且在创建派生类对象时，其内存中的 `_vptr` 指向的是派生类的虚表。
- 当将派生类对象的地址赋值给基类指针时，基类指针可以通过这个 `_vptr` 间接访问到派生类对象的虚函数。由于多态性的动态绑定机制，基类指针调用虚函数时，会根据派生类对象的 `_vptr` 来确定调用哪个函数，即使基类指针的类型是基类

# RTTI:

Run-Time Type Information: 运行时类型检查

主要内容包括

1. typeid信息，用于获取表达式类型信息

2. **`dynamic_cast`操作符**：用于对象指针或引用安全地转换为目标类型的指针或引用。它会在运行时检查转换是否合法，如果转换不合法（例如，将一个非派生类指针转换为基类指针以外的类型），则会返回空指针（对于指针转换）或者抛出`std::bad_cast`异常（对于引用转换）。

### 虚表（Virtual Table）

虚表是在编译时由编译器为每个具有虚函数的类创建的。对于每个类，虚表是一个函数指针数组，其中包含了类的所有虚函数的指针。当一个对象被创建时，它的隐藏的虚指针（通常称为vptr）会被设置为指向其类的虚表。这个过程不涉及RTTI，因为它是在编译时确定的。

### 运行时类型识别（RTTI）

RTTI是C++提供的运行时机制，它允许程序查询对象的实际类型。这通常通过以下两种方式实现：

1. `typeid`操作符：返回一个`type_info`对象的引用，可以用来获取对象的类型信息。

2. `dynamic_cast`操作符：用于将对象指针或引用安全地转换为目标类型的指针或引用，并且在转换过程中进行类型检查。

   

   

   **维护虚表的过程不涉及RTTI，而在使用`dynamic_cast`进行类型转换时才会运行RTTI。普通的指针赋值操作，即使涉及到继承和多态，也不会触发RTTI。**

# 面试概率题







# 设计模式之单例模式：

单例实现要点：

1. 构造函数和析构函数是私有的，不允许外部生成和释放。

2. 静态成员变量和敬爱返回单里的成员函数
3. 禁用拷贝构造和赋值运算符



在下图的代码中，`static Singleton2 * GetInstance()`这个函数使用的懒汉模式 ， 就是用的时候我才new出来一个使用哈哈哈哈哈。然后这里使用atexit（）调用了一个函数，意味着在退出的时候调用。

![image-20240830205748091](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240830205748091.png)

![image-20240830210138523](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240830210138523.png)

以上的是单线程模式下



多线程模式线程安全的单例模式

什么是线程安全：获取这个对象，包括对象的生成，这个过程他是线程安全的

![image-20240830211308411](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240830211308411.png)

图中解释有误，第二个线程来的时候不会再创建了，因为此时_instance 不为空了。所以61行是为了并发线程进来的时候只允许一个线程创建。其他直接到69行return 

但是以上的操作 在多处理器的情况下是有问题的。多核心下，会进行CPU指令重排 有可能会只执行1,3，此时是没有构造的。

那如何解决？原子操作 内存屏障。 原子操作就是在多核处理器下，其他处理器不会看到原子操作内部过程，只能看到是否开始和结束。

如何解决指令重排？指令重排我们没法改变，但是我们可以不让其影响_Instance

![image-20240830212601671](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240830212601671.png)





# 平衡二叉树：

所有节点左右子树高度差的绝对值小于等于1.

如果二叉树失衡，则可以进行左旋（逆时针）或者右旋（顺时针）操作平衡二叉树。

旋转之后与旋转之前的中序遍历相同

失衡LL型，绕平衡因子为1的节点，旋转失衡因子为2的节点。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200124064.png" alt="image-20240902200124064" style="zoom:50%;" />

RR型，失衡节点因为右孩子的右子树发生失衡

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200350892.png" alt="image-20240902200350892" style="zoom: 33%;" />

LR型，	失衡节点因为左孩子的右子树发生失衡

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200723887.png" alt="image-20240902200723887" style="zoom:33%;" />

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200752275.png" alt="image-20240902200752275" style="zoom:33%;" />

RL型 ，失衡节点因为右孩子的左子树发生失衡

调整方法，先右旋右孩子，在左旋失衡节点。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200918633.png" alt="image-20240902200918633" style="zoom:33%;" />

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902200936058.png" alt="image-20240902200936058" style="zoom:25%;" />

如果插入之后有多个节点失衡，那么只需要调整离叶子节点最近的节点即可

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240902201312517.png" alt="image-20240902201312517" style="zoom:50%;" />

### 二叉树的构建

构建的时候跟二叉搜索树相同，依次插入节点。但是会根据是否失衡不断旋转

### 二叉树的删除操作

删除之后需要检查是否有失衡，然后再调整，而且调整可能不仅仅有一次，调整之后可能因为调整了子树，祖先节点可能会再次失衡



### 左右旋：

左旋就是爷爷变成左孙子，右旋就是爷爷变成右孙子。

# 为什么析构函数必须是虚函数？

在实现多态的时候，当基类操作派生类，在析构的时候防止只析构基类，而不析构派生类情况的发生。



1. 资源正确释放：派生类可能会拓展基类增加新变量或者资源。如果基类析构函数不是虚函数，那么删除派生类对象的时候，仅会调用基类析构函数，而派生类中新增的资源无法得到释放，可能导致资源泄露。

2. 多态性支持：C++中多态性允许基类指针或引用来操作派生类对象。当基类析构函数是虚函数，无论是指针或引用实际对象是什么类型的都能保证调用正确的析构函数，从而维护多态性。

3. 删除操作的安全性：当使用基类指针删除派生类指针的时候，如果基类析构函数是虚函数，那么会先调用派生类的析构函数，然后调用基类析构函数，**这样可以避免因为错误的析构顺序而导致的问题。**
4.  继承链的完整：继承连中每个类的析构函数，都应该负责释放自己的资源，如果析构函数不是虚函数，那么在继承链中的资源释放将会是不完整的。

## 如果基类函数不是虚函数，子类函数同名函数实现的功能不同，这属于多态吗？

不属于：

1. 基类必须存在虚函：基类中的函数必须被声明为虚函数（使用virtual关键字），这样可以通过基类指针或者引用调用函数，会根据对象的实际类型决定调用哪个函数版本。
2. 子类必须重写基类的虚函数。

如果基类中没有虚函数，Base父类，Derived子类，有同名函数Func,

Base* b = new Derived(); b->Func(); *// 调用的是Base类的Func实现，不是Derived类的Func实现*

# const 和#Define的使用区别

## const关键字

1. 类型安全：声明变量时会指定类型，因此他是类型安全的。
2. 作用域：const变量具有作用域，可以是局部，也可以是全局的。
3. 内存分配：const变量在编译时分配内存，在运行时是可以访问的。
4. 初始化：声明时必须初始化；
5. 调试：调试的时候可以查看其值。
6. 不可修改：一旦初始化之后其值不可以修改。

const int MAX_SIZE = 100;

## #define 预处理指令

1. 文本替代：#define 工作原理是在编译前进行文本替换，不涉及类型检查
2. 无作用域：宏定义没有作用域限制，他们在定义之后直到文章末尾或被#undef取消定义之前都是有效的
3. 无内存分配：只是简单文本替换，不分配内存
4. 参数：宏可以接受参数，并对参数进行操作
5. 调试：调试时无法查看其值，因为在预处理阶段就被替换掉了。
6. 可修改性：任何地方都可以重新定义

#define MAX_size 100；

### `#define` 在编译之前会做什么

在编译之前，`#define` 宏定义会在预处理阶段执行以下操作：

1. **文本替换**：预处理器会查找所有宏定义的实例，并将其替换为定义时的文本。
2. **参数展开**：如果宏定义包含参数，预处理器还会对传递给宏的参数进行替换和展开。
3. **条件编译**：宏定义常用于条件编译指令（如 `#ifdef`、`#ifndef`、`#if`），以控制哪些代码应该被编译。
4. **文件包含**：宏定义可以用于 `#include` 指令，以包含其他文件。

由于宏定义是在预处理阶段进行的文本替换，它不会进行任何类型检查或内存分配，因此在编译器开始编译代码之前，所有宏相关的文本已经被替换完成。这使得宏在某些情况下可以提供性能优势（因为不需要内存访问），但也可能导致代码难以理解和维护，并且容易出错。





# 为什么map底层用的是红黑树（RBT）而不是平衡二叉树（AVL）？

**平衡性:**红黑树是自平衡的二叉搜索树，他通过确保从根节点到叶子节点最长的可能路径不多于最短的可能路径的两倍长来保持树的平衡。这种平衡保证了红黑树的基本操作（如插，删，查）时间复杂度为O(logN).虽然AVL树也是平衡二叉树，但是要维护平衡的条件更严格，这导致在插删操作的时候需要更多的旋转操作来维持平衡。

**性能：**维护平衡的成本低，通过少量颜色变换和旋转即可保持平衡，而AVL需要更多的旋转操作。这使得插删操作下红黑树的性能更优

**实现复杂性：**红黑树的实现相对简单。	他只需要额外维护一个颜色信息，而二叉树需要维护每个节点的平衡因子，这增加了实现的复杂性。

**稳定性：**红黑树在插入和删除操作后，原有的元素顺序不会改变，这为map和Set提供了稳定的迭代顺序。而AVL在维护平衡的时候，可能改变元素顺序



突然明白了为什么说map是有序的，unordered_map是无序的，因为map是自平衡的二叉搜索树，所以他的中序遍历是有序的，而在map中，他对应的节点就是key_value的key，key必须是有序的，所以说map是有序的。而unordered_map是哈希表，所以没有顺序性可言。



# map和Set的区别，底层实现是什么？

都是C++容器类，底层实现都是红黑树（RB-Tree）

Set是const的，不允许修改元素的值；map是允许修改Value的，但是不允许修改key，如果真的要修改，就要将这个键删掉，然后调节平衡，再插入修改后的键值。如此一来，会导致与被删除节点相关联的迭代器会失效。这是因为删除操作会改变红黑树的结构，可能导致节点移动或者重新分配，从而使原有迭代器不再指向有效位置

除了被删除节点本身的迭代器失效，其他迭代器通常有效。原因是红黑树在删除节点的时候通常会保持其他节点相对位置不变，以维护整体的平衡性和有序性

在实际编程中，为了避免迭代器失效的问题，通常建议在修改容器（如插入或删除元素）之后，重新获取迭代器。

同时，使用`map`和`set`等容器时，应当注意它们提供的成员函数，如`erase`，这些函数会返回下一个有效迭代器，可以在删除操作后安全地继续使用。

如何使用：

```c++

map<int,string>myMap;
map<int,string>::iterator it=myMap.begin();
while(it!=myMap.end()){
if(someCondition(it->first)){
    it=myMap.erase(it);
}else{
    it++;
}
}
```

这个函数中，使用while循环整个map，每次迭代检查当前元素是否满足someCondition函数。如果条件为真，我们调用erase方法删除函数**，并且更新迭代器it,为erase方法返回下一个有效迭代器。**如果不为真，我们通过++it迭代器后移，继续检查下一个元素，这样可以在遍历过程安全的删除元素，而不会导致迭代器失效。





# 智能指针



## unique_ptr:

```c++
#include <iostream>
#include <vector>
#include <memory>
using namespace std;
class AA{

public:

    string m_name;
    AA(){cout<<"调用构造函数"<<endl;};
    AA(const string &name):m_name(name){cout<<"调用构造函数AA("<<m_name<<")"<<endl;};
    ~AA(){cout<<"调用析构函数~AA"<<m_name<<endl;}
};

//不能传值，这个是5. 的演示
 void func(unique_ptr<AA>pp){
        cout<<(*pp).m_name<<endl;
    };
     void func2(unique_ptr<AA>&pp){
        cout<<"传引用成功，传的名字为："<<pp->m_name<<endl;
    };
 
unique_ptr<AA> fun3(){
    unique_ptr<AA>pp(new AA("沉鱼"));
    unique_ptr<AA>pp2(new AA("luoyan"));
    return pp;
 }

//------8.第八点
 void func4(const AA *a){
    cout<<a->m_name<<endl;//just裸指针，删除权不收该函数控制
 }
 void func5( AA *a){
    cout<<a->m_name<<endl;
    delete a;     //删除权自己控制
 }
 void func6(const unique_ptr<AA> &a){
        cout<<a->m_name<<endl;
 }
 void func7(unique_ptr<AA> a){
        cout<<a->m_name<<endl;
 };



int main(){

//    AA *p =new AA("西施");
 //   unique_ptr<AA> puq(p);
    //这行代码意思是：  模板参数为被管理的普通指针的对象类型为AA,被管理的指针是p,p指向被管理的地址
    //合起来就是，让智能指针puq来管理对象

//运行之后，发现调用了AA的析构函数，为啥？因为智能指针是类，他有析构函数 ，在他的析构函数中使用了 delete语句

  //  cout<<"智能指针重载了解引用符，使用解引用符，访问属性："<<(*puq).m_name<<endl;
 //   cout<<"使用箭头访问："<<puq->m_name<<endl;
    
//用已存在的地址存储智能指针，并不是一个好的初始化方法，还有其他初始化方法，如下：

  //  unique_ptr<AA>puq1(new AA("__貂蝉"));//方法2：分配内存并初始化
  //  cout<<"我知道是你，貂蝉！："<<puq1->m_name<<endl;

   // unique_ptr<AA>P0=make_unique<AA>("吕布");//方法3： C++14标准

//  1.  unique_ptr的构造函数是explicit，不能用于转换，也就是，不能将普通指针直接赋值给智能指针，防止程序员犯错

    // AA *q=new AA("刘邦");
    // unique_ptr<AA> PO1=q;
    // unique_ptr<AA>PO1=new AA("项羽");//这两种都是错的


    //2. 拷贝构造函数也被禁用，如下：
    //unique_ptr<AA>pu2=puq;

    //3. 不能使用赋值函数，对unique_ptr进行复制；
   // unique_ptr<AA>pu3;
    //pu3=puq;

//为啥呢？因为unique_ptr是独享的智能指针一个unique_ptr只对一个对象负责
//如果unique_ptr允许复制，则会出现多个unique_ptr指向同一块内存的情况，其中一个unique_ptr过期的时候，就会释放资源，
//其他的unique_ptr过期又释放内存，就会变成野指针。

//4.不要将一个裸指针（普通指针）用多个智能指针管理
   // 程序会异常，所以还是这种方法最好：
  // unique_ptr<AA>PK(new AA("张飞"));


//5.不能传值，因为拷贝构造函数被删除
  //func(PK);//报错，不能传值
//func2(PK);//没有报错，可以传引用



    //6. 多次赋值的时候，由于unique_ptr只能掌控一个指针，所以他会释放掉之前的内存，然后控制一个资源

   // unique_ptr<AA>pp(new AA("沉鱼1"));
   // unique_ptr<AA>pp2;
   // pp2=unique_ptr<AA>(new AA("沉鱼2"));
   // PP2=fun3();//在这里就是将函数返回的pp赋值给了pp2,但是直接pp2=pp,这样是行不通的，神奇，明明赋值函数已经删除1了，还能通过函数赋值，这是因为编译器为unique_ptr做了特殊处理。
//---------------------------------------------------
//7.unique_ptr被赋值为nullptr之后会被释放资源。
    //   cout<<"调用前，没空"<<endl;

    //   pp=nullptr;
    //   if(pp==nullptr){
    //      cout<<"调用后，空了"<<endl;
    //   };
      
//---------------------------------------------------
//8. release()将释放对原始指针的控制权，将unique_ptr置空，返回裸指针。
//move的应用,在func7中对原始指针的控制权，交给了形参，形参就是a的引用。

unique_ptr<AA>pp(new AA("沉鱼1"));

   // cout<<"函数调用开始"<<endl;

    // func4(pp.get());
    // func5(pp.release());
    //func6(pp);    函数需要一个unique_ptr，函数不会对这个unique_ptr负责
    //func7(move(pp));//函  数需要一个unique_ptr，函数会对这个unique_ptr负责

    //cout<<"函数调用结束"<<endl;
//---------------------------------------------------

//9.reset()函数
//pp.reset();//释放pp对象指向的资源对象
//pp.reset(nullptr);//释放pp对象指向的资源对象
//pp.reset(new AA("羞花"));///释放pp对象指向的资源对象,并指向新对象

//cout<<pp->m_name<<endl;
//---------------------------------------------------

//10. swap()交换两个unique_ptr的智能指针的控制权

//11.和普通指针一样，智能指针也具有多态的性质，父类指针，指向子类对象的时候，调用函数，依然可以实现多态

    
  //12、 unique_ptr不是绝对安全的，使用exit（）的时候，全局的智能指针可以释放，但是局部的智能指针不会释放
//13 .智能指针数组是模板类，可以使用数组。、
    
    
unique_ptr<AA[]>pp(new AA[2]);


// unique_ptr<AA>mingzi(new AA[3]);

pp[0].m_name="xishi1";
pp[1].m_name="xishi2";

cout<<pp[0].m_name<<endl;
cout<<pp[1].m_name<<endl;

unique_ptr<AA[]>pt1(new AA[3]{string("杨玉环"),string("程序员"),string("资本家")});
//直接初始化的话，需要在后面附上变量类型以及变量内容。
for(int i=0;i<3;i++){
    cout<<pt1[i].m_name<<endl;
};

    
    //delete p;
}
```

内存泄露：指的是程序在申请内存后未能正确释放这部分内存，导致这部分内存变得不可用，但同时又无法被程序的其他部分重新利用。

在程序终止的时候操作系统会自动释放程序所占用的内存，包括栈堆全局变量等所有内存空间，因此即使出现内存泄漏，也会在程序结束时释放。

但是良好的编程习惯是重要的，及时释放不需要的内存，使用智能指针等自动内存管理工具以防止内存泄露

智能指针学习到的地方：https://www.bilibili.com/video/BV1gV4y1G7fH?t=408.4&p=2



unique_ptr在使用数组的时候，数组创建时为父类对象，其中若是有子类元素，则都视为父类类型，



```c++
#include <iostream>
#include <memory>
#include <string>
using namespace std;

class AA {
public:
    string name;
    AA() { cout << "无参构造函数调用" << endl; };
    AA(string n) : name(n) { cout << "含参构造--name:" << n << endl; };
    virtual ~AA() {
        cout << name << "调用了析构函数" << endl;
    };

    virtual void print1() { cout << "杀人越货" << endl; };
    void attack() { cout << "攻击一下" << endl; };
private:
    int age;
};

class BB : public AA {
public:
    BB() {};
    BB(string n) : AA(n) { cout << "含参构造函数name:" << endl; };
    void print1() override {
        cout << "忽有狂徒夜磨刀，帝星飘摇荧惑高" << endl;
    }
};

int main() {
    unique_ptr<AA[]> Arr2(new AA[3]{AA("赵子龙"), BB("李银明"), AA("刘邦")});
//这是AA类型的unique_ptr数组
    for (int i = 0; i < 3; i++) {
        cout << Arr2[i].name << "，他说:";
        Arr2[i].print1();
    }

    return 0;
}

```





问题原因：`std::unique_ptr<AA[]>` 会根据数组的元素类型进行分配和管理，但并不会为每个元素动态选择虚函数，因此它的行为不完全符合多态。

```c++
这是因为AA类和BB类之间存在虚函数的重写机制（print1函数）。虽然BB类重写了print1()，但在使用unique_ptr<AA[]>时，编译器默认会认为数组中存储的对象类型为AA，即使你在初始化数组时传递了BB对象。

问题的根本原因：
当你使用unique_ptr<AA[]>声明一个AA类型的数组时，数组元素的类型会被处理为AA类型，即使传入的是BB类型的对象，编译器也会在数组元素的生命周期内，视其为AA类型的对象，导致虚函数的调用机制（动态绑定）没有正确生效。
在初始化数组时，AA("赵子龙")和AA("刘邦")分别是AA类对象，因此它们的print1()方法调用会输出“杀人越货”，即使BB类重写了该方法。
解决办法：
如果希望在数组中正确地使用BB类的重写方法，可以考虑使用指针（例如unique_ptr<AA>）来管理这些对象，而不是使用数组。这样，编译器会正确处理虚函数的动态绑定。

改为使用std::vector<std::unique_ptr<AA>>，它支持多态，并且能够正确调用虚函数。

    vector<unique_ptr<AA>> Arr2;
    Arr2.push_back(make_unique<AA>("赵子龙"));
    Arr2.push_back(make_unique<BB>("李银明"));
    Arr2.push_back(make_unique<AA>("刘邦"));

    for (int i = 0; i < 3; i++) {
        cout << Arr2[i]->name << "，他说:";
        Arr2[i]->print1();
    }

//Vector是容器，动态数组，但是可以存储指针


```



### 多态小妙招：

数组中AA类型数组中，如果有BB类型数据，调用其析构函数不会触发多态，因为没有指针指向

如果是指针数组，则可以，因为，父类指针指向了子类对象，触发多态



**多态：静态多态（编译时多态）和动态多态（运行时多态）**

#### 静态多态：

是在编译阶段就确定要调用的函数，主要通过**函数重载和运算符重载** 

函数重载是同名函数有不同根据类型或者返回值的不同实现不同的功能

运算符重载，**使程序员能够为自定义类型重新定义运算符的行为。**比如一个类person中有两个变量，a,b.我想实现两个person实例小明、小红相加的时候他们各自的a,b相加（或者小明的a*10之后与小红相加等运算逻辑） 小明 +小红的时候就会按照重载运算符之后的逻辑进行处理。 

#### 运行时多态（动态多态）：

运行时多态是在程序运行时才确定要调用的函数，主要通过**虚函数和继承来实现**。

虚函数和继承：

虚函数是在基类中使用 `virtual` 关键字声明的函数，派生类可以重写（override）这些函数。通过基类指针或引用调用虚函数时，实际调用的是派生类中重写的函数，这取决于指针或引用所指向的对象的实际类型。

## const在函数中的作用：

1. 当在函数的参数前面的时候：	他告诉编译器或者程序员，这个参数在函数体内不会被修改。这对函数调用者来说很重要，因为他确保了传递给函数的对象不会被意外修改。

```c++
void print (const string &str){
	cout<<str<<endl;
}
```

2. 参数后的const 当一个成员函数	被声明为const后，	他告诉编译器和程序员，该函数不会修改对象状态。（通常用于get函数等地方）这意味着函数不能修改任何非静态成员变量，也不能调用任何非const成员函数：例如：

```c++
class MyClass{
    	public：
            int value;
    		MyClass(int n):value(n){};
    		void setValue(int cc){
                value=cc;
            }
       		void getValue()const {
                return value;
            }
    //getValue函数被声明为const，这意味着它不会修改MyClass对象的状态。
}
```

3. 返回值使用const ：编写的函数希望调用者知道，该返回值不应该被修改，那么应该返回一个const 引用或者指针

   如果返回的值可以修改应该返回非const引用或者指针

举例：

```c++
class Person
{
    private:
    string  name;
public:
    Person();
    Person(string n):name(n){};
    ~Person(){cout<<"调用析构函数"<<endl;};

    const string& getName() {
        return name;
    }
};
int main(){
    Person p1("小鱼");
    p1.getName();
    cout<<p1.getName()<<endl;
cout<<"------"<<endl;
    p1.getName()="feiji1";//返回值使用const的话 真不让修改啊，编译就会报错，牛
 cout<<p1.getName()<<endl;
}

```

auto 关键字用于自动推导。他可以用于任何变量类型。

以下是使用场景：

​		

```c++
1.普通变量
auto x = 42; // x的类型是int
auto y = 3.14; // y的类型是double
2.函数返回值
auto Add(int a,int b){
    return a+b;
}
3.STL元素
vector<int>vec={1,2,3};
for(auto& i:vec){
    cout<<i<<endl;
}
4.智能指针
unique_ptr<int>pp1(new int(42));
auto i=pp1.get();
//i的类型是int *
5.自定义类型
    
    
6.指针或者引用
    int a = 5;
auto& ref = a; // ref的类型是int&
auto ptr = &a; // ptr的类型是int*


```

有没有想过为什么auto后面会有&这个符号?

**`auto`关键字与`&`符号结合使用时，表示创建一个引用。**

使用场景：

```C++
1. 迭代器中使用，修改迭代器中的元素的值时：
    
    vector<int>vec={1,2,3};

	for(auto& i:vec){
         i*=2;
		cout<<i<<endl;   返回确实是2,4,6，如果没有使用&，在循环内输出是246，在循环外面返回的是123（未更改该地址中的内容）
    }
2. 函数参数，当你想要函数接收任何类型参数的时候，可以使用auto&
    template<typename T>
		void process(T& value) {
    // 处理value
}
3. 当你想返回值类型为引用的时候，可以使用auto&
    
    auto& RE(){
    static int value=42;
    return value;
}
    
```

## shared_ptr:

shared_ptr共享他指向的对象，多个shared_ptr可以指向相同的对象，在内部采用计数的机制实现。当新的shared_ptr与对象关联时，引用计数加1，当shared_ptr超出作用域的时候，引用计数减1，当引用计数减为0时，说明没有任何shared_ptr与对象关联，则释放该对象

使用和unique_ptr一致，成员函数use_count()可以获取引用计数的个数。

get（）函数获取裸指针

 

```c++
#include <iostream>
#include <vector>
#include <memory>
using namespace std;
class AA{

public:

    string m_name;
    AA(){cout<<"调用构造函数"<<endl;};
    AA(const string &name):m_name(name){cout<<"调用构造函数AA("<<m_name<<")"<<endl;};
    ~AA(){cout<<"调用析构函数~AA"<<m_name<<endl;}
};
int main(){

        // shared_ptr<AA>pp1(new AA("吕布"));
        AA* p=new AA ("吕布");
        shared_ptr<AA>pp1(p);


        cout<<pp1->m_name<<endl;
        cout<<"use count:"<<pp1.use_count()<<endl;
        
        //shared_ptr能使用拷贝构造，拷贝构造之后，引用计数器数量加1.

        shared_ptr<AA>pp2(pp1);//使用拷贝构造函数，创建引用
        cout<<"use count:"<<pp2.use_count()<<endl;//这里输出2和上面输出1是因为顺序执行还不知道有其他共享指针引用了同一块地址
         cout<<"pp2 point to memory address:"<<pp2.get()<<endl;

        shared_ptr<AA>pp3=pp1;
        //支持赋值，左值的shared_ptr值减一，右边的shared_ptr将加1（如果引用的不是同一块地址的共享指针）


        cout<<"use count:"<<pp3.use_count()<<endl;//pp3的输出也是3，说明确实是共享了数量，换句话说，就是三个共享指针，指向同一块地址
        //然后三个任意一个打印use_count()的引用个数都是一样的，此时该地址被几个指针共同引用
        cout<<"pp3 point to memory address:"<<pp3.get()<<endl;
        //get()方法返回裸指针

    
    AA* p=new AA ("吕布");
    shared_ptr<AA>pp1(p);

    shared_ptr<AA>pp2=pp1;
    shared_ptr<AA>pp3=pp2;
    cout<<"use count:"<<pp3.use_count()<<endl;

    AA* d=new AA ("刘备");
    shared_ptr<AA>pd1(d);
    shared_ptr<AA>pd2(pd1);
    shared_ptr<AA>pd3(pd1);
    cout<<"use count:"<<pd3.use_count()<<endl;
3.赋值的时候，左值共享指针数量减1，右值共享指针数量加1.
    pd3=pp2;
    cout<<"吕布的use count:"<<pp3.use_count()<<endl;
    cout<<" --"<<endl;
    cout<<"刘备的use count:"<<pd1.use_count()<<endl;
     cout<<"刘备的use count:"<<pd2.use_count()<<endl;
      cout<<"刘备的use count:"<<pd3.use_count()<<endl;//为什么会是4呢？
      //因为将吕布绑到pd3上了，pd3现在是吕布，所以输出pd3.count()的时候，实际上输出的是吕布的引用，就是4

      pd2=pp1;//赋值就是左边的共享指针指向地址发生变化，被赋值为右边的。所以左边原本的共享指针减少，右边原本指向地址的指针增加1、
      pd1=pp1;
     // 此时没有共享指针指向pd2,所以刘备会被析构；
       cout<<"刘备的use count:"<<pd2.use_count()<<endl;//这个打印的应该是6，对的


    
   4. unique_ptr和shared_ptr都不支持指针的运算（--，++，-，+）
   5. 实际开发中一般不关心use_count（）的值，因为资源大于0，资源就一直存在，如果为0，那就释放资源。 
   6. shared_ptr没有release（）函数，多个shared_ptr不关心共享一个内存，释放管理权可不是一个共享指针说了算的。
   7.可以使用move（）转移对原始指针的控制权，unique_ptr可以转换成shared_ptr，反之不行
       
   unique_ptr<AA>UN1(new AA("独占指针"));
    //  怎么将其转换呢？unique_ptr转为shared_ptr?
    
    shared_ptr<AA>SN2(move(UN1));
    cout<<UN1->m_name<<endl;//会报错，因为move资源之后，unique_ptr指向变为空了，也就释放资源了
    cout<<SN2.use_count()<<endl;
   
   8.reset（）改变与资源的关联关系，如果成员函数内部无参，则资源引用减1，有参的话，则关联新资源
       
   9.实际应用中，能使用独占指针unique_ptr就不要用shared_ptr，unique_ptr的效率更高，占用资源更少
       
}
```

#### 智能指针删除器：

默认情况下：智能指针过期的时候，使用delete原始指针，释放资源。

管理员可以自定义删除器，改变智能指针释放资源的行为。

删除器可以是全局函数，仿函数，和lambda表达式，形参为原始指针。、、





```c++
       

//3种自定义删除器——————————
void deleteFunc(AA *a){//自定义普通函数删除器
    cout<<"调用了自定义删除器deleteFunc"<<endl;
    delete a;
};

struct deleteClass{
    void operator()(AA *a){
        cout<<"自定义删除器，仿函数"<<endl;
        delete a;
    }   
};

auto deletelambda=[](AA *a){
    cout<<"Lambda"<<endl;
    delete a;
};


//三种删除器应用，给shared_ptr指定删除器，因为有的时候我们想自定义删除某些资源或者打印某些log的时候会很方便-------
       shared_ptr<AA>Sp1(new AA("悔恨"),deleteFunc);
        shared_ptr<AA>Sp2(new AA("进取"),deleteClass());
       shared_ptr<AA>Sp3(new AA("飞羽"),deletelambda);

```

unique_ptr的自定义删除器会麻烦一些。





## weak_ptr:

是什么？为了配合shared_ptr而引入的**，他指向一个由shared_ptr管理的资源，但是不影响资源的生命周期**，也就是说，**将weak_ptr绑定到一个shared_ptr的时候，不会改变shared_ptr的引用计数。**

无论是否有weak_ptr指向，如果最后一个指向资源的shared_ptr被销毁，资源就会被释放。

也就是说，weak_ptr是shared_ptr的一个助手，而不是一个智能指针；

如何使用weak_ptr？

weak_ptr没有重载*和->，所以不能直接访问资源。

有以下**成员函数**：

operator=（）；将shared_ptr或weak_ptr赋值给weak_ptr；

expired（）；判断他指向的资源是否过期（或已被销毁）。

lock（）；返回shared_ptr，如果资源已过期，返回空的shared_ptr。，G该成员函数是线程安全的属于原子操作。

reset（）；将当前weak_ptr置空；

swap（）；交换。



使用lock（）函数将其提升为线程安全。

```c++

```





## 智能指针应用——面试加强

首先使用智能指针的核心目标是管理动态分配的内存，避免手动管理内存带来的内存泄漏问题。其关键机制是引用计数，通过记录有多少个智能指针指向同一对象，





什么时候使用shared_ptr，什么时候使用weak_ptr，为什么？

​	对于shared_ptr，我们在需要对引用资源进行指针之间的共享的时候，通常使用shared_ptr。

使用shared_ptr，weak_ptr会作为辅助指针使用，那什么时候使用weak_ptr呢？首先就是两种使用场景：

1. 使用shared_ptr的时候，要解决循环引用。此时要使用weak_ptr打破循环引用的问题，避免资源无法释放。（大家都能达到）
2. 其他什么时候用到weak_ptr，不考虑环的情况下，能一直使用weak_ptr吗？ 当我们需要使用一块共享的内存的时候，我们会首先想到使用shared_ptr，从业务逻辑上来讲，如果我们只是使用此资源，不应该对此资源的生命周期造成影响的话，此时应该使用weak_ptr，而不是shared_ptr

针对使用weak_ptr代替使用shared_ptr的情况有两个原因回答下：

1. 从性能上讲，weak_ptr的性能开销通常比shared_ptr低，因为他不会增加引用计数，只是提供对被管理对象的引用，此时使用shared_ptr有助于提高性能
2. 从逻辑上讲，shared_ptr引用了一个对象，就会延长他的生命周期，对他引用计数造成影响,如果业务逻辑本身就是对其进行一个观察的话，我们也应该使用weak_ptr来保证不影响其资源的引用计数

**观察者模式中使用weak_ptr**

​	那既然不影响生命周期，那在使用观察者模式的时候，被观察对象可以存储观察者的weak_ptr,**以便在状态变化时通知它们**,结合蓝图中的事件分发器就是知道了。需要通知观察者，所以要有观察者的引用。这样可以避免观察者对被观察的对象的生命周期产生影响

​	妈的，刚才有点不理解为什么要在被观察对象中存储观察者的weak_ptr，因为当其发生变化的时候要告诉观察者发生变化了，而且不会因为存储的是观察者的shared_ptr而对被观察者的生命周期造成影响。



## 如果让你实现智能指针，你会怎么实现呢？

会从以下几个角度考虑设计：



##### 类设计层面：

如果要实现智能指针肯定通过**类模板**形式实现，这样可以支持不同类型的管理。

定义类的时候类成员设置指针指向实际对象的指针，引用计数指针等。

构造函数用于初始化智能指针，并为引用计数分配内存。

##### 相关拷贝函数和运算符重载的设计：

拷贝构造函数：用于创建一个新指针没，新指针创建的时候，需要将引用计数增加

赋值运算符：赋值运算符处理的时候需要考虑当前智能指针的引用计数和目标智能指针的引用计数

析构函数：再析构智能指针的时候，减少引用计数。如果此时引用计数变为0，则释放对象和引用计数指针

解引用运算符和箭头运算符重载：



多线程环境下的考虑‘：

使用互斥锁或者原子操作：

使用原子类型来实现引用计数，原子操作是线程安全的，无需额外的锁机制.



##### 设计中考虑智能指针中的移动语义：

unique_ptr是 独占式智能指针，同一时间只能有一个unique_ptr指向某个对象，他支持移动语义，不支持拷贝函数，因为靠本能违反了独占性原则

shared_ptr的话，可以多个shared_ptr指向同一个对象，用引用计数管理对象的生命周期。既支持拷贝语义，也支持移动语义.



​	在移动构造函数和移动赋值运算符中，直接将资源所有权从源对象转化到目标对象，避免了引用计数的增加和减少操作，提高了性能，同时将源对象的指针置空，确保源对象不再拥有资源

---



		在C++中，`std::shared_ptr` 是一种智能指针，用于管理动态分配的对象，它使用引用计数（reference count）来跟踪有多少个 `std::shared_ptr` 实例共享同一个对象。当引用计数变为0时，所管理的对象会被自动删除。

##### 引用计数Reference count的定义位置

​		引用计数的具体实现细节因不同的编译器和标准库实现而异，但通常会在一个独立的控制块（control block）中定义。控制块是一个动态分配的结构体，它除了包含引用计数外，还可能包含弱引用计数（用于 `std::weak_ptr`）、删除器（deleter）和分配器（allocator）等信息。

##### 为什么引用计数是指针

引用计数之所以是指针，主要有以下几个原因：

1. **共享性**：多个 `std::shared_ptr` 实例需要共享同一个引用计数，以便它们能够正确地跟踪对同一个对象的引用数量。通过使用指针，所有共享同一个对象的 `std::shared_ptr` 实例都可以访问同一个引用计数，从而实现对引用计数的同步更新。

2. **动态分配**：引用计数通常存储在控制块中，而控制块是动态分配的。使用指针可以方便地管理这个动态分配的内存，确保所有 `std::shared_ptr` 实例都能正确地访问和更新引用计数。

3. **灵活性**：使用指针可以在运行时动态地分配和释放引用计数，使得 `std::shared_ptr` 可以处理不同类型的对象和删除器。

### 示例代码

下面是一个简单的示例，展示了 `std::shared_ptr` 的基本用法：

```cpp
#include <iostream>
#include <memory>

int main() {
    // 创建一个std::shared_ptr实例，指向一个整数对象
    std::shared_ptr<int> ptr1 = std::make_shared<int>(42);

    // 输出引用计数
    std::cout << "ptr1 use count: " << ptr1.use_count() << std::endl;

    // 创建另一个std::shared_ptr实例，共享同一个整数对象
    std::shared_ptr<int> ptr2 = ptr1;

    // 输出引用计数
    std::cout << "ptr1 use count: " << ptr1.use_count() << std::endl;
    std::cout << "ptr2 use count: " << ptr2.use_count() << std::endl;

    // 释放ptr2
    ptr2.reset();

    // 输出引用计数
    std::cout << "ptr1 use count: " << ptr1.use_count() << std::endl;

    return 0;
}
```

### 代码解释

- `std::make_shared<int>(42)`：创建一个 `std::shared_ptr` 实例，指向一个值为42的整数对象，并自动分配控制块。
- `ptr1.use_count()`：返回 `ptr1` 的引用计数，即有多少个 `std::shared_ptr` 实例共享同一个对象。
- `ptr2 = ptr1`：创建另一个 `std::shared_ptr` 实例 `ptr2`，并让它共享 `ptr1` 所管理的对象。此时，引用计数会增加。
- `ptr2.reset()`：释放 `ptr2` 对对象的引用，引用计数会减少。

通过这种方式，`std::shared_ptr` 可以自动管理对象的生命周期，确保对象在没有任何引用时被正确删除。



# 栈溢出

定义：指的是程序向某个变量中写入的字节数超过了这个变量本身所申请的字节数，因而导致栈中与其相邻的变量的值被改变。

栈溢出的原因：

1.局部数组过大。函数中数组过大时，有时可能导致堆栈溢出。局部变量是存储在栈中的，因此这个很好理解。解决这个问题有两个方法：1.增大栈空间，2.改用动态分配，使用堆heap而不是栈stack

2.递归调用次数太多，因为递归函数在运行是会进行压栈操作，当压栈次数过多时，会导致堆栈溢出。

3,。指针或者数组越界，这个最常见，比如字符串拷贝或者处理用户输入等



堆和栈的区别，以及为什么栈要更快？

堆是低地址到高地址拓展；栈是由高地址到低地址拓展

堆是有程序员手动申请和释放；栈中内存是有OS自动申请释放，存放参数，局部变量等内存。





# lambda表达式：







### 回调函数：





首先明白一个道理，函数指针的作用和普通指针一样，用于参数的传递，函数指针也一样的，不然指来指去没意思

函数的地址： 内存中的起始地址，如果把函数地址作为参数传递给函数，那么就能灵活调用函数了。



#### 回调函数定义：

回调函数就是把一个函数的函数名作为另一个函数的参数。

#### 使用函数指针的三个步骤：

1. 声明函数指针。

2. 让函数指针指向函数的地址；
3. 通过函数指针调用函数



声明普通指针的时候，必须提供指针类型，同样，声明函数指针也必须提供函数类型

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20241224180415178.png" alt="image-20241224180415178" style="zoom:50%;" />

~~函数类型不就什么int ,bool,float，以及各种自定义的对象类型~~

函数类型是指返回值类型和参数列表（参数列表的形参类型） ！函数名和形参名不是用于区分函数类型，

正确的函数指针声明： 返回值类型（*函数指针名）（参数类型，参数类型....）;

![image-20241224181325158](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20241224181325158.png)

int ssss(int ew,string str){ cout<<"asdas"<<endl; return 0;} 这个函数的类型和newPtrFun函数是一样的。（多思考下为什么我会这么说，这是一个结论哦）

代码实现怎么实现，最初讲了函数指针的使用步骤：

声明函数指针，指向函数地址，通过函数指针调用函数



那写函数的目的就是把某些功能封装起来呗。 表白神器函数中有一个函数指针 在中间 实现的是 ...函数的多态？

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20241224183123669.png" alt="image-20241224183123669" style="zoom:50%;" />

个性化表白这部分代码是不确定的，每个人都有不同的方式，在调用表白神器的时候，把个性化表白函数的地址传递进去，即可，那么实现的功能就是，不同的函数名有不同的表白方式

函数指针回调函数和利用函数名调用函数的意义不一样。

{ 用函数指针可以调用**相同函数类型的不同函数**，调用者函数将回调函数指针嵌入到调用者函数中，调用者函数1提供了主体流程和框架，具体功能有回调函数实现 }



特点： 我们在写调用者函数的时候只需要关注回调函数的种类，不需要关注回调函数功能。

回调函数的应用场景：

多线程和网络



```c++

void skeletal(int no,string name){
    cout<<no<<"//"<<name<<endl;
}

void sks1(int no,string name){
    cout << "------" << endl;
    cout << "who is the " << name << ",the " << no << endl;
    cout << "------" << endl;
}

//封装一个调用体 ，函数指针直接当做参数放进此函数
void Body(void (*sks)(int, string), int a, string b)
{ // param is other func ptr
    for (int i = 0; i < 3; i++)
    {
        cout << "head" << endl;
    }
    sks(a, b);
    cout << "ending..." << endl;
}
int main(){
//     声明函数指针的时候，*后面是指针名
//     void (*sss)();//声明函数指针
//     int (*newName)(int,string);//声明函数指针
//     void (*zz_ptrNewName)(int);
//     void(*sharelove)(int,string);//声明函数指针
//     sharelove=newFun;//函数指针指向函数地址
//     sharelove(3,"小于");//函数指针名调用函数
//     ShowLoveWay(way2);
//  ShowLoveWay(way3);

Body(sks1,9,"joker");
Body(skeletal,1,"我的发");
    return 0;
}; 
```



菱形继承（https://cloud.tencent.com/developer/article/1784496）：

缺点：

- 数据冗余：D会保存两份A的内容
- 访问不确定性（二义性）：D不知道是以B为中介访问A,还是以C为中介

解决：

- 数据冗余：通过下面的“虚继承”解决
- 二义性：通过作用域访问符：：来明确调用。虚继承也可以解决这个问题

# 递归：



### 三部曲：

1. **确定函数参数和返回值**

2. **确定终止条件**

3. **确定函数的主要逻辑**

   

递归就是往下递，往回归，二叉树中为什么要往下递，是因为我需要他往回归这个过程，所以才从根节点向下递。

比如二叉树中，要找公共祖先这一题中，往下递是因为要从叶子结点回归，因为要保存某个节点，所以需要返回值且类型是TreeNode * ，

所以递归，递的过程算深度搜索吗? 算

什么向下递呢？调用自身函数的时候，将参数设置为自己的孩子节点时就进行了“递”操作。



二叉树：递归究竟什么时候需要返回值，什么时候不需要返回值呢？

结论是，看是否需要遍历整个二叉树。

如果需要遍历整颗树，递归函数就不能有返回值。如果需要遍历某一条固定路线，递归函数就一定要有返回值

疑问：遍历整个树，为什么不能有返回值？为什么遍历某一固定路线的时候就一定要有返回值呢？

那参数呢？对于参数：在确定递归函数的参数的时候没必要一次性确定，当我们需要什么的时候再进行填写参数就行





# 右值引用，移动语义：



左值：lvalue  右值：rvalue

左值：locator value 也就是能够代表一块存储区域的表达式 

能够获取这个**表达式的引用**或者**取地址**，也就能够判断其为左值

C++11之后，分为了泛左值和纯右值

泛左值（glvalue）包括lvalue和xvalue(即将消亡的值)

纯右值prvalue

![image-20250219233733674](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250219233733674.png)

以下例子都属于左值（lvalue）：

```c++
 int a=8; //a属于左值
 double m = 1.5 ;//m属于左值
 int *p=new int{5};
 *p=4;//解引用符号表达式，是左值
 int b[5];
 b[1]=10;//属于不可更改的左值（为什么）
 
 int &r=a;
 变量r是引用，属于左值引用，左值引用的赋值运算符右侧必须是左值，a也是左值 
 
 类的数据成员也是左值
 struct S{
 int id;
 }s1;
 s1.id=3;
 例如，s1的成员变量id就是个左值
 
 返回引用的函数 ，此类函数的调用表达式也是左值
 
 
 int &refNumber(){
 	static int n=1;
 	return n;
 }
 int main(){
 	reNumber()=5;
 	cout<<refNuber()<<endl;
 }
 
reNumber()也属于左值

```

以下例子属于纯右值：

prvalue:

需要将计算结果存放在临时中间对象中的都是prvalue

例如：

```c++
int m=3,n=1;
int x,y,z;
double d;

x=m+n; //这里的m+n
y=-m;// 这里的-m
z=n+2;//这里的n+2;都属于纯右值
d=(double)z;//这种类型转换也属于prvalue


还有一些也属于prvalue
    非静态类成员函数
    枚举
    数组
    this指针
    lambda表达式
    一些内置运算符等：a++,a&&b,a+b等等都是prvalue、
```

现在跳过xvalue了解下、

## 右值引用

```c++
int a=5;
int&b=a;//左值引用
此例子中，b是a的引用，a是左值，所以b是左值引用

int &&c=5;

此例子中 &c是引用 5是右值 所以这里&c是右值引用
但是！注意！
变量c是一个绑定右值的引用，右值引用只能绑定右值，不能绑定左值；
    此时如果 int a=5;
	int &&c=a;
	这就是错误的（因为a是左值）
        
 有了右值引用就能将左值引用和右值引用分开来
例如两个重载函数func
 void func(int& a){
        cout<<"调用左值引用函数"<<endl;
    }
void func(int&& a){
	   cout<<"调用右值引用函数"<<endl;	
}
int a=5;

func(a);//输出调用左值引用
func(5);//输出调用右值引用
        

```

## 移动语义：

通过重载函数能够区分左值引用和右值引用 接下来就能够实现移动语义了。



首先做一个缓存函数

```c++
class CharBuffer{
public:
	CharBuffer(unsigned int nSize):m_buff(new char[nSize]),m_Size(nSize){
		cout<<"普通构造函数"<<endl;
   	~CharBuffer(){
        delete[]m_buff;
        cout<<"析构函数"<<endl;
    }
}
private:
	char* m_buff;
	unsigned int m_size;

}
```

**普通构造函数**构造的时候，分配了一块nSize大小的地址，并且指针m_buff指向这块地址
CharBuffer   m1(100);

但是**拷贝构造**的时候
 CharBuffer  m2(m1);
此时因为我们没有定义拷贝构造函数，所以编译器会**自动生成拷贝构造函数**。
    而自动生成的拷贝构造函数会**将成员变量都复制一遍**
  会有造成两个错误的地方：

- m2中的指针和m1中的指向一样，指向了同一块地址。

- 销毁两个对象的时候，会调用析构函数，对同一块内存区域释放两次    

 



所以为了解决这个问题，创建了自己的**拷贝构造函数**、**拷贝赋值运算符的重载函数**

![image-20250220005548669](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220005548669.png)	为了实现移动语义还需要**移动构造函数**和**移动赋值运算符重载函数**

跟拷贝的很像，但是参数类型为右值引用，并且去除了const 的限制符，因为我们要在函数中对传入对象进行修改

![image-20250220005718117](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220005718117.png)

![image-20250220010055472](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220010055472.png)

移动赋值运算符单独的一张图片（因为没截全）





#### 来个例子

```c++
CharBuffer buff2{CharBuffer(100)};
```

这里应该是首先  有100参数的调用构造函数进行匿名构造，构造完成说明这是个右值，如果参数是右值的话，对于buff2来说就是调用移动构造函数来创建buff2.

理论确实是这样，但是实际运行的话，会发现

![image-20250220010701661](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220010701661.png)

为啥只创建了一次对象且未调用构造函数？因为C++11的拷贝省略CopyElision,编译器在一些情况下会省略临时对象创建这一步，而是直接在目标存储位置构造该对象（编译器这样做的目的是省略临时对象的创建，到目标对象的复制或移动、临时对象的销毁等这些不必要的开销）

#### 例子2：

在函数直接返回值的情况下，临时对象的创建也会被省略

```c++
CharBuffer generate(int n){
	return CharBuffer(n);
	}
int main(){
		CharBuffer buff2=generate(100);
}
```

 ![image-20250220011355970](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220011355970.png)这叫 返回值优化



#### 例子3：

具名返回值优化NRVO

```c++
CharBuffer generate_nv(int n){
		CharBuffer buf(n);//局部变量，函数返回时会被赋值给一个临时对象，然后生命周期结束被销毁  若没有临时对象，也至少应该赋值到buff2，然后被销毁
		cout<<&buf<<endl;
		return buf;
}
int main(){
CharBuffer buff2=generate_nv(100);
cout<<&buff2<<endl;
}

```

输出结果是，&buf也被输出了。

![image-20250220012429305](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220012429305.png)

使用编译器指令关闭后输出结果为：

![image-20250220012459402](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220012459402.png)

调试编译器中参数关闭返回值优化和nrvo

![image-20250220012530199](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20250220012530199.png)



为什么要创建移动构造函数？为什么创建运算符重载函数



> 2025年2月20日01:38:51  明天继续看 【一起来学C++  34. 右值引用和移动语义-xvalue, std::move】 https://www.bilibili.com/video/BV1JRUHYxEsy/?share_source=copy_web&vd_source=448909cdfe7ff87e464eb123889e9d9a







> 2025年2月20日 我来公司了 ，开始学移动语义

```c++
通过
CharBufer buff1(100);
CharBuff buff2 =buff1;
输出结果是，调用了拷贝构造函数
```

#### 例子1（使用移动构造函数）：

```c++
CharBuff buff1(100);
CharBuff buff2=std::move(buff1);
通过输出结果发现，调用的是移动构造函数，所以std::move()表达式     属于右值表达式
    
    由于我们知道他实际上没有移动什么而是静态转换，所以效果等同于
    CharBuffer buff=static_cast<CharBuffer&&>(buff1);
```



std::move()属于右值表达式

std::move做了什么？实际上并没有移动什么，只是做了静态转换，将左值转换成了右值引用



#### 例子2（使用移动赋值运算符）：

```C++
int main(){
CharBuffer buff2(10);

    {
        CharBuffer buff1(100);
        buff2=std::move(buff1);
    }
这里使用的就是移动赋值运算符，移动语句将左值buff1转化为右值引用,然后移动赋值运算符将其赋值给buff2
    移动赋值之后，buff1中的指针成为空指针
    
    
    我们还可以看到作用域{}
    buff1的生命周期在{}内，离开了大括号之后，buff1就被销毁了，而buff2仍然保有buff1的资源，也就是说，对象buff2在buff1即将消亡之前将buff1资源转移到自己这里。这里就是xvalue（即将消亡的值）。我们使用move函数获取他的右值引用，从而将他的资源回收再利用
}
```





**移动语义作用：将哪些将要因为离开作用域而销毁的资源，如果我们还希望能够继续使用它不想让他销毁，那么在他销毁之前使用move函数将其转换成右值引用，再通过移动赋值运算符赋值，转移给需要的新对象，我们就能继续使用那块资源**

### **移动语义总结：**

移动语义的目的：通过std::move 将即将销毁的对象资源转换成右值引用，通过移动赋值运算符转移给需要它的新对象，避免资源复制

`std::move` 将**左值**转换成**右值引用**，**触发移动构造函数或者移动赋值函数**





移动语义主要目的是提高程序在处理资源管理时的效率，特别是对象之间进行资源转移的场景下：

资源管理方面：

​		**避免不必要的拷贝：**传统的对象赋值或传参操作，会将一个对象内容复制到另一个对象，这可能会涉及分配内存、赋值数据、打开文件句柄，网络资源连接等操作。。特别是对那些大型的对象或者动态分配的对象，这种操作可能造成大量性能开销，移动语义就是为了解决这个问题。



语法实现角度：

​		**移动构造函数和移动赋值运算符**：类需要定义自己的移动构造函数和移动赋值函数来实现移动语义。移动构造函数通常接受一个右值引用参数，在函数内部完成资源从源对象到新对象的转移。移动赋值运算符也是一样的，只是这个针对的是赋值操作



​		**移动构造函数使用情景**：在初始化一个新的左值对象的时候，如果使用的是 右值来初始化，那么就会调用移动构造函数。（终于搞搞清楚了）

​		**移动复制运算符使用情景是**   将一个右值的资源转移到已经存在的左值对象中。也就是说，当一个已经存在的左值对象被一个右值赋值的时候，就会调用移动赋值运算符。

#### 代码展示    

 移动构造函数 和移动赋值运算符，都是将右值的变量给左值，然后将右值中的变量和指针置空。

```c++
#include<iostream>

using namespace std;

class MyClass{
    public:
    int*data;
    普通构造函数
    MyClass():data(new int[100]){
        cout<<"普通构造函数"<<endl;
    };
    移动构造函数(参数得是右值)
    MyClass(MyClass&&other)noexcept:data(other.data){
        other.data=nullptr;
        cout<<" copy construct function"<<endl;
    }
    移动赋值运算符重载（参数得是右值）
    MyClass& operator=(MyClass&&other) noexcept{
        if(this!=&other){
            delete []data;！！！当执行移动赋值操作时，意味着要将另一个对象（右值）的资源转移给当前对象，为了避免内存泄漏，需要先释放当前对象现有的资源。！！！还有很重要的一点，这里必须执行delete[]data,因为如果不释放这段资源，在将右值资源赋给左值之后，左值之前的内存空间将无法访问和释放，会造成内存泄露。
    
            data=other.data;
            other.data=nullptr;
            cout<<"移动赋值运算符"<<endl;
        }
        return *this;
    }
    ~MyClass(){
        cout<<"调用析构函数"<<endl;
        delete []data;
    }
};

 int main()
 {
    MyClass obj1;
    MyClass obj2;
    obj1=std::move(obj2);
    MyClass obj3(std::move(obj1));使用move左值转右值
     
    此时obj1虽然资源被转移走了，但是因为没有出自己的作用域，所以生命周期没有结束，
    那些在出自己作用域之前的对象，如果还要使用这个对象的资源，此时使用move资源转移之后，出作用域了，那个对象才会被释放
   
    return 0;
 }

```

移动赋值运算符重载的函数中：

操作顺序1.清空当前左值对象的原有资源，2.将右值资源赋值给左值，3.将右值资源置空



**为什么一定要清空左值中的动态资源：**

因为如果不释放这段资源，在将右值资源赋给左值之后，左值之前的内存空间将无法访问和释放，会造成内存泄露。



#### 移动语义问题：

使用unique_ptr的时候：unique可以被赋值吗？可以转移吗？

unique_ptr是语义上独占所有权的指针，他不允许被赋值或者拷贝，只能通过移动语义将所有权转移。



通常什么情况下使用std::move呢？

性能优化的时候，避免一些不必要的开销。比如使用move在容器间转移元素，而不是复制。

当我们确定一个对象不再使用的时候，当有一个临时对象或者确定不再需要这个对象的时候，就可以将其内容转移到另一个对象，最经典的就是unique_ptr来改变资源的所有权的时候

 

当一个对象给被使用std::move的时候，你再使用它会产生什么问题？

被移动对象进入一个**合法但是未被指定**的状态，他是合法数据，可以被正常析构，可以被重新赋值，但是如果这是使用的话可能会导致崩溃，或者其他一些未知的行为。

  

move究竟移动了什么？

移动的资源的所有权：

https://www.bilibili.com/video/BV1gm421H7bc?t=1658.0



### 移动语义注意事项

虽然移动语义带来了明显的性能提升，但也有一些注意事项：

- **安全性:** 确保移动后不再访问原始对象，因为它的状态已被清除。
- **异常安全:** 实现移动构造函数和移动赋值运算符时要小心处理异常情况，确保资源的安全性。
- **兼容性:** 并非所有的类都实现了有效的移动构造函数和移动赋值运算符，默认生成的版本可能不够高效。



#### 例子3：

```c++
CharBuffer& assign(CharBuffer& to ,const CharBuffer &from ){
    to =from ;
    return to;
}
CharBuffer&assign(CharBuffer &to ,CharBuffer&& from ){
	to =from;
    return to;
}
int main(){
    CharBuffer buff1;
    CharBuffer buff2(100);
    assign(buff1,std:move(buff2));
}
```

此例子中，并未调用移动赋值运算符，而是拷贝赋值运算符，为什么？

这个涉及类型传递问题。需要使用**完美转发**

## 完美转发

 依旧看例子3，我们在主函数中调用了参数为右值类型的那个函数，但是函数处理过程并没有显示移动赋值运算

为什么？因为在assign函数中不管参数是什么类型（左值引用或者右值引用）函数内部都有一个**局部变量from去承载它**，可以获取他的地址，所以他是个左值





std::forward ()完美转发，如果参数过多都需要区分左右值的话，下面例子是有两个参数，那么需要应对四个重载函数。

<img src="C:\Users\atarkli\AppData\Roaming\Typora\typora-user-images\image-20250220135657199.png" alt="image-20250220135657199" style="zoom:50%;" />

因此，使用模板来替代

```c++
void f(const string& s){
cout<<"左值："<<s<<endl;
}
void f(string &&s){
    cout<<"右值:"<<s<<endl;
}


template<class T>
void g(T&& v){
	f(std::forward<T>(V));
}
该模板的参数T是右值引用的形式 
  这种被称为万能引用，或者叫转发引用 （universal reference ）
    他并不是真正的右值引用，因为它既可以   绑定左值，也可以绑定右值  
    
    只有在函数模板中，这种形式的参数才能叫做万能引用，如果出现在非函数模板的函数体代码中，则不是万能引用，而属于右值引用
    在这里g的参数会根据实际类型被推到成相应的类型
    
    如果实参是左值，则被推导为实参类型的引用
    如果实参是右值，被推到为非引用类型
    
    例如
    g(string("helllo"));
	这是T被推导为string形式
        即此时模板就被实例化为
        void g(string &&v){
        f(std::forawrd<string>(v);)
    };
    
int main(){
    g(string("hello1"));
    string s1("hello2");
    g(s1);
}
此时输出
    右值：hello1
    左值：hello2
```



#### 那forward做了什么呢？



作用：保持原来的值属性不变。啥意思呢？通俗的讲就是，如果原来的值是左值，经std::forward处理后该值还是左值；如果原来的值是右值，经std::forward处理后它还是右值。

![image-20250220141844063](C:\Users\atarkli\AppData\Roaming\Typora\typora-user-images\image-20250220141844063.png)

 `T&& forward(typename std::remove_reference<T>::typr&t)` 就是将模板类型T去掉引用之后的实际类型



有点不理解  但是这里forward总的来说做的事情和move一样，只是做了个类型转换

还是看例子来理解，

```c++
void g(string &&v){
      f(std::forawrd<string>(v);)
    };
```

##### 当g的实参是右值的时候

- 在f函数中传入的是个左值（因为这个v在函数内部属于局部变量了）

- 所以在这里使用了forward函数左值引用的重载函数

- 将forward的模板参数替换成string之后，forward函数实例化就变成了：

  ```c++
  string &&forward<string>(string &t)  noexcept{
   return static_cast<string&&>(t);  
   }
  ```

  也就是说，我们给函数g传入实参是右值引用，forward返回的也是右值引用



##### 当g的实参是左值时

forward会将模板转换成 实参的引用形式 

```c++
string s1("hello2");
g(s1);

也就是此时 T=string&
模板实例化就是
void g<string& &&v){
	f(std::forward<string&>(v));
}
```

此时发现，模板实例化之后会有引用的引用形式，此时就是**引用折叠**的处理方式

```
T& &    T&  左值引用的左值引用被折叠为左值引用
T& &&   T&
T&& &   T&
T&& &&  T&&  右值引用右值引用才是折叠为右值引用
```







# 递归

递与归：

递归函数是否有返回值与是否使用其“归”的能力之间有一定关联，但不能简单地绝对化理解，以下是具体分析：

### 无返回值递归函数
- 没有返回值的递归函数在很多情况下确实**侧重于利用“递”的过程来**完成某种操作或遍历等任务，比如对树形结构进行遍历，在“递”的过程中访问每个节点并执行一些操作（如打印节点值），并不一定依赖“归”过程带回特定的值来进行后续处理。但这并不意味着完全没有用到“归”的能力，在递归调用结束后，程序会从最深层的递归调用逐步返回到上层调用，这个过程依然存在“归”，只是没有显式地通过返回值来利用它。例如，在打印一个目录下所有文件和子目录的名称时，可以使用无返回值的递归函数，在“递”的过程中深入到各个子目录，在“归”的过程中实际上完成了对整个目录结构的遍历操作。

### 有返回值递归函数
- 有返回值的递归函数通常会**更直接地利用“归”的能力**。在“归”的过程中，返回值会携带计算结果或状态等信息，从最底层的递归调用逐步向上传递，以便在更高层的调用中进行进一步的处理或计算。比如计算阶乘的递归函数，在“递”的过程中不断将问题规模缩小，直到达到终止条件，然后在“归”的过程中通过返回值将每一步的计算结果逐步传递回来，最终得到完整的阶乘结果。

所以，递归函数无论有无返回值，都包含“递”和“归”两个过程，只是有返回值的递归函数更便于在“归”的过程中传递和利用计算结果等信息，而无返回值的递归函数可能更多地是利用递归的整体过程来完成一些不需要显式返回结果的任务，但不能说没有用到“归”的能力。





# 数组

在 C++ 里，数组名是一个常量指针，它指向数组的首元素。一旦数组被定义，它的地址就固定下来了，不能通过赋值操作让它指向另一个数组。所以，当你尝试执行 `a = other.a;` 时，编译器会报错，提示 “必须是个可修改的左值”，因为数组名 `a` 不能作为赋值运算符的左操作数。

-----

2025年2月27日

### 代码命名规范： 

函数命名用动词形态，数组之类的用复数形式表示



### 异步编程 ：

async   [*ə´zɪŋk*] 异步

<img src="C:\Users\atarkli\Desktop\gitProisity\demo1\Work_space_2025\screenshot\异步名言.jpg" alt="异步名言" style="zoom:50%;" />









# 排序：

堆排序：

因为之前做的二叉树的题多一些，所以想学下堆排序。

- 首先堆是完全二叉树。
- 大根堆：任意节点的值都大于它的左右孩子。 那么根节点就是所有节点的最大值
- 小根堆：任意节点的值都小于等于他的左右孩子。那么根节点就是所有节点的最小值
- 那么怎么存储的的呢？首先堆完全二叉树嘛，所以用**顺序存储**的话，不会有空间上的浪费
- 从下标1开始，下标0就省略了，为什么省略？**你猜一个节点的左右孩子怎么表示** nums[i]的左孩子是nums[2i]，右孩子是nums[2i+1]
- 还是得记住`/`是除完之如果有小数，那就省略了**小鼠部分**（好记），也就是向下取整。、

## 大根堆：

给你一个数组，将其从小到大进行排序。

![屏幕截图 2025-02-28 144418](..\Work_space_2025\screenshot\屏幕截图 2025-02-28 144418.png)

- 可以将这个数组视为完全二叉树，但是它不是堆啊，所以要将这个数组变成堆的形式

- 问题来了，怎么将其变成堆呢？倒着将数组调整成堆的形式。因为叶子节点没有左右子树，所以以他们为根的子树自然就是堆。（为什么倒着呢？因为这样可以确定遍历到某个节点的时候他的左右子树都已经调整成堆了）

- 从最后一个非叶子节点开始进行调整。那么这个节点的下标是多少？如果该数组长度为n,那么最后一个非叶子节点就是最后一个节点的父亲，所以**最后一个非叶子节点下标为`n/2`**。

- 然后从n/2开始向前调整子树的堆，此时n/2需要--，即`n/2--`，每次都判断 n/2--的子树是不是符合堆，符合就继续--，不符合就调整之后再--

注意，与子树交换之后，可能破坏了子树所在树的堆，他换下去之后还要继续递归调整。

> 在存储完全二叉树的数组中，判断一个节点的左右孩子是否存在，应该是判断索引是否越界，即`2*i<nums.size()和2*i+1<nums.size()`

- 当调整到最后的时候，只剩余一个元素了，就不用调整了，他就是最大的。

- 然后将此值与最后一个位置进行交换，然后一直进行堆调整，每次堆调整都能找到一个最大值，所以最后得出的数组就是从小到大的数组

### 总结：

时间复杂度：对于有序无序的数组，因为都需要将最后一个元素挪到堆顶，所以无论是最好最坏的的情况都是**nlogn的复杂度**.

**辅助空间复杂度是O（1）**：因为仅仅在交换的时候才使用辅助空间

堆排序是**不稳定**的，因为每次都将最后一个元素提上去了。

注意，我们**逻辑上**使用的是堆结构，但是**实际存储**的时候使用的是数组。



```c++

残缺代码， 关于堆调整的，写着写着写不下去了
#include<iostream>
#include<vector>
using namespace std;
class Solution{

    public:
     vector <int> heapsort(vector<int>& nums){
        int n=nums.size()/2;
        int i=n;
        while(n--){
            //这里需要判断左右节点是不是都存在，都存在的时候比较两孩子二点大小
            //如果只存在一个节点的话，就将父节点和度的孩子比较即可
            if(nums[2*i]!=nullptr||nums[2*i+1]){
                    这里我有思路错误，判断子树是否存在应该是通过判断索引是不是小于n,而不是不等与空指针，这俩数据类型都不一样。
            }
            if(nums[i]<nums[2*i]&&nums[2*i]>nums[2*i+1]){
                swap(nums[i],nums[2*i]);
            }else if(nums[i]<nums[2*i+1]&&nums[2*i]<nums[2*i+1]){
                swap(nums[i],nums[2*i+1]);
            }
            这里整体判断逻辑是没太大问题的，但是处理的时候有问题了，不用判断左右子树的值，。。通过代码处理顺序就可以实现两个值的间接判断
            在后续正确代码中我会指出。
            
        }
     }
};


int main(){

}
```



### 正确代码：

```C++
#include<iostream>
#include<vector>
using namespace std;
class Solution{

    public:
     vector <int> heapsort(vector<int>& nums){
        int n=nums.size();
        for(int i=n/2-1;i>=0;i--){
            heapify(nums,n,i);
        };

        for(int i=n-1;i>0;i--){
            swap(nums[0],nums[i]);
            heapify(nums,i,0); //因为最后一个上去了，从根调整就行。注意中间的参数是i了，每次不要管最后一个
        //因为最后一个已经有序了
        }
        return nums;
  
     }
     private:
    void heapify(vector<int>&nums,int n,int i){ //注意这个函数只是进行了调整操作，不返回任何值
            int largest = i;
            int left=2*i+1;
            int right =2*i+2;
        //以下的逻辑在顺序上就实现了，哪个更大就将那个赋值给largest ,注意，if中使用的是nums[largest]了
            if(left<n && nums[largest]<nums[left]){
                largest=left;
            }
            if(right<n&& nums[largest]<nums[right]){
                largest=right;
            }
            
            if(largest != i){//这里就是赋值之后不相同了
                swap(nums[i],nums[largest]);
                heapify(nums,n,largest);//这里就是将交换之后的节点继续递归调整
            };
     }
    };


int main(){
    std::vector<int> nums = {12, 11, 13, 5, 6, 7};
    Solution sol;
    std::vector<int> sorted_nums = sol.heapsort(nums);

    // 输出排序后的数组
    for (int num : sorted_nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

所有该注意的地方，全都在代码注释中：

- 排序的时候，注意第二个参数n,传入的参数是i，因为每次都有一个值是有序的了，所以每次我们需要调整的值就少一个
- heapify中对于左右子树的大小判断，隐藏在了代码顺序中和代码比较时的`nums[largest]<nums[left]`注意不是nums[i]和子树对比，这个很巧妙，因为这样如果左子树比根节点大，那么就将其`largest=left`  然后根据**顺序执行**能够再进一次if跟右子树的值对比。很巧妙。
- `if(largest!=i)`说明确实子树比根大，那就swap交换嘛，交换完之后继续 向下递归进行堆调整



#### 用于堆调整的函数：

heapify函数：

递归调用函数，对索引为largest的节点（视为根）的子树进行调整，通过这种递归方式，实现了**每次对进行交换的节点**的**子树**进行调整











### 关于函数入栈时候的参数是从左到右还是从右到左？

​	C和C++中，通常函数入栈参数是从右到左，这样可以方便的实现可变参数函数，如printf.

静态转换和动态类型转换的区别是什么？



​    函数返回值是引用类型有什么作用？

​	返回值是引用的话，可以少一次拷贝的操作。返回引用



代码的编译过程（面试出现的次数比较多的）  【已解决】



进程之间，线程之间是如何通信的？



为什么不能有虚构造函数呢？虚函数有什么优缺点呢？





# 类的内存布局是什么样的呢？

- 类只有成员变量占用内存。静态成员变量不占用内部内存，函数不占用内部内存.如果有虚函数，每个类对象都会有虚函数指针Vptr(占用一个指针大小)，Vptr指向一个虚函数表，虚函数表中记录跟想标记为virtual的函数。
- 如果子类覆盖父类虚函数，该子类虚表里的虚函数就会被子类替换。
- 虚表什么时候被确定的呢？**编译完成之后确定**的，大小和布局，加载时的内存位置都被确定。
- 如果时多继承的话，你猜有几个虚指针，有几个虚表？有几个基类就有几个虚指针，就有几个虚表。

# 虚继承

虚继承什么玩意儿？看了才知道是为了解决菱形继承的问题

![img](https://pic2.zhimg.com/v2-e6c42339a7277be29035b73937bb0c4d_1440w.jpg)

```c++
class A { public: int num; };
class B : public A {};
class C : public A {};
class D : public B, public C {};
```



类D访问继承父类成员num的时候，D.num有二义性。需要指定访问链条： D.B::num 即访问继承自B的类A中的num成员，且C中也存在一份存储在不同内存的同名成员，造成**数据冗余**



可以使用虚继承解决此问题，将类B,C虚继承自类A,类D同时继承B，C时只会保存一份A，原理其实就是**虚继承的派生类中各自保存一个类A的地址**。

```c++
class A{
public: 
	int m_a;
};

class B:virtual public A{
public:
	int m_b;

}
class C:virtual public A{
public:
    int m_c;
}

class D: public B,public C{
public:
	int m_d;    
}
```

类D的对象d的内存空间中自上而下存储 

B::vbptr，

B::m_b,

C::vbptr,

C::m_c，

m_d，

以及**m_a**

- 一般继承中，基类变量排列在派生类变量之前，但是出现**虚继承**的时候，虚基类的变量会被放置在最后，编译器会为派生类为维护一张**虚基类表**。B和C中都会有自己的虚基类表。用于记录他们和A之间的关系。

每个含有虚基类的派生类都有一个虚基类指针（vbptr）,这个指针指向虚基类表(vbt),后者提供必要信息以便正确访问虚基类成员。

虚基类表中都有什么呢？

​	**虚基类表是一个静态表格，其中包含以下信息：**

	1. 虚基类的偏移量 ，这是核心信息，指示了虚基类在其所在对象中的确切位置。例如D的对象中，A的偏移量会告诉编译器如何找到A的成员。
	1. 构造函数调用信息：虚基类表中有可能提供虚基类构造函数入口地址，以便对象构造的时候正确初始化
	1. 析构函数调用信息：虚基类表中有可能有虚基类函数的入口地址，以便对象构造的时候正确初始化虚基类
	1. RTTI信息：某些编译器中可能包含运行时类型信息，用于支持Dynamic_cast等操作。

```
D对象的VBT:

+--------------------+
| Offset to A   | -> 偏移到 A成员的偏移量
+--------------------+
| Constructor Info   | -> 构造函数信息（可选）
+--------------------+
| Destructor Info    | -> 析构函数信息（可选）
+--------------------+
| RTTI Info          | -> 运行时类型信息（可选）
+--------------------+
```

- 字节对齐：类A按照四字节对齐，类BCD因为指针的存在使用8字节对齐。

D是普通继承B和C过来的，由于虚继承关系，D会在内存中布局中包含B和C的虚基表指针等信息，用于正确访问虚基类A的成员

当`D`类普通继承`B`类和`C`类时，`D`对象的内存布局里会包含`B`类和`C`类的虚基表指针。这些指针指向的虚基表并非直接指向`A`的成员信息，而是存储了从`B`或者`C`到`A`的偏移量。通过这些偏移量，在运行时就能从`B`或者`C`的实例出发，正确定位到`A`的成员。

> 注意偏移的位置，A的实例只有一份，且存在在D中，所以如果B和C的实例想要访问A中的实例信息，就要向D中唯一一份A的实例的地方偏移
>

### 虚基表＋虚函数

那如果虚继承的派生类中有虚函数会怎么样呢？

##### 如果是单虚继承中有虚函数：

```c++
class A{
public:
	virtual void func(){ cout<<"A Fun"<<endl;}
}
class B: virtual public A{
public:
	virtual void func(){ cout<<" B fun"<<endl;}
}
```

此时sizeof（B) 输出的是16 刚好两个指针的大小，也就是一个虚基类表指针，一个虚函数表指针  虚基类指针在虚函数指针之前

内存分布：

![img](https://pic4.zhimg.com/v2-dcf52b91c25ff339f26f458ccb94f695_1440w.jpg)



如果虚继承中有自己的新的虚函数：

```c++
class B: virtual public A{
public:
	virtual void func(){ cout<<" B fun"<<endl;}
	virtual void func1(){
		cout<<"new fuinction about B"<<endl;
	}
}
```

![img](https://pic3.zhimg.com/v2-a968728c5096e1c874cae7346324fc7c_1440w.jpg)

此时发现是开辟出一个新虚函数用于存储虚函数func1。派生类独有的虚函数不会储存在基类虚函数之后，而是会**额外开辟一张虚函数表**，并且**额外分配一个虚函数指针**，这个派生类独自的徐函数指针地址排在虚基类指针之前。



# 内存对齐的原理和意义：

原理：有一定的对齐规则，

意义：减少CPU的读取次数，提高效率

```


char str[]="hello";

sizeof(str) = 6
```

因为数组最后还有一个占用空间的\0 



# C++中代码的编译过程都包含哪些步骤？

> 预处理、编译、汇编、链接。

### 步骤：

1. **预处理**：

   - 处理内容：预处理器根据源文件中的预处理指令，对源代码进行初步处理。 **常见任务：宏定义展开、头文件包含、条件编译、注释去除 。**   比如将`#include`指令包含的头文件内容插入到源文件中、展开#define 宏、处理#ifdef #ifindef #endif等条件编译指令。
   - 作用：通过预处理，可以将多个源文件和头文件整合成一个便于编译的单元，同时**根据不同的条件生成不同版本的代码**，提高代码的可维护性和可移植性。UE源码中就有宏的作用是editor时才显示的变量、函数等
   - 输出结果，预处理后的代码通常是以`.i`为后缀进行保存

2. **编译**：

   - 编译器对预处理后的代码进行词法分析语法分析语义分析。**每个源文件独立编译。**词法分析将字符流转换为**单词序列**，语法分析根据语法规则**构建语法树**，语义分析检查语法结构的语义正确性。然后编译器将代码转换为中间形式，并进行优化，如：常量折叠、死代码消除、循环展开等，以提高代码执行效率。
   - 作用：确保C++代码符合C++语法和语义规则，将高级语言转换为计算机能够理解的中间形式，为后续目标机器码做准备。
   - 输出结果：输出汇编语言，通常以`.s`为后缀保存

3. **汇编**：

   - 处理内容：汇编器将汇编代码转化为机器码。他根据汇编指令和目标机器的指令架构，将每条汇编指令翻译成对应的二进制机器码并生成目标文件。目标文件包含代码段、数据段等信息，但是其中**符号引用**尚未被解析

   - > 哦~那是不是C++代码通过汇编之后变为机器码，才在内存中的结构才定下来的？
     >
     > 答案：并不是，汇编只是将汇编代码转换为机器码，形成目标文件，而目标文件里的符号引用和地址并未确定。在链接阶段，代码在内存中的结构才会确定下来

   - 作用：将汇编语言转换为机器可执行的二进制指令，生成与目标架构相关的目标文件

   - 输出结果：生成目标文件，通常以`.o`,`.obj`作为后缀保存

4. **链接**：

   - 处理内容：链接器会将多个**目标文件**以及**所需的库文件**链接在一起，形成可执行文件或共享库。链接过程主要完成符号解析和地址重定位工作。符号解析是将目标文件中用的符号引用与其他文件的符号定义进行匹配，地址重定位则是根据最终的内存布局，调整目标文件中指令和数据地址，确保程序在运行时能够正确访问。
   - 作用：能够将各个目标文件和库文件整合为一个完整可执行文件或者共享库，解决符号引用你和地址分配问题，使程序能够在目标机器上正确执行
   - 输出结果：可执行结果或共享库，通常为`.exe` `.dll` 文件，在Linux为`.so`



### 注意：

汇编阶段产生的目标文件确实保存着机器码。但是这些**机器码和特定符号关联**，也就是说这个阶段，他们**未能和最终内存地址绑定**，链接的时候进行绑定。

存在**符号引用**的原因：汇编阶段，编译器并不知道所有符号（函数名，全局变量名）的具体内存地址。这是因为程序可能由多个文件组成，各个源文件分别编译为目标文件，每个文件中的符号引用可能指向其他目标文件中定义的符号。

​	例如：在一个多文件项目中，file1.c调用了file2.c中定义的函数func。在编译file1.c时，编译器只知道有一个名为 fun 的函数需要被调用，但是并不知道这个函数的实际内存地址。因此，它会在生成的目标文件中标记出这一点：“这里有一个对 fun 的调用，请稍后再告诉我它的真正地址。”，所以它只能用`符号func`来表示这个函数调用



### 符号引用：

符号引用并不是以机器码的形式表示可执行操作，目标文件中，符号引用通常以一种特殊的数据结构来记录，具体来说，目标文件中有符号表，重定向表。符号表记录所有符合的名称和相关位置，**重定向表则记录了所需要在链接时需要进行地址调整的位置**。链接器处理目标文件时，会根据符号表和重定向表来解析符号引用，将符号替换为实际的内存地址。















# 指针

指针的大小只跟计算机系统架构有关，与指针所指向的对象类型无关。因为指针本质是一个变量，其存储的的是内存地址 。

无论指针指向的是一个字符，整数，结构体，指针大小只取决于系统的位数。

- 在32位操作系统中，指针大小是4字节
- 在64位操作系统中，指针大小是8字节



#### 那我有问题了，声明指针时：

**为什么要有不同类型呢？是为了区分同名指针吗？**

不是的，即使指针本身的大小固定不变，但我们仍然需要不同类型的指针。

#### 原因：

**1，** 指针类型决定了指针在**解引用时访问内存的方式**和**访问的字节数**。这与指针指向对象的数据类型有关

```c++
比如:
int num=10;
int* pInt=&num; 整型指针
char* pChar=(Char*)&num; 字符指针【为啥地址取地址符前要（char*）?

 std::cout << sizeof(*pInt) << std::endl;  // 输出 4，因为 *pInt 访问的是一个 int 类型的数据
 std::cout << sizeof(*pChar) << std::endl; // 输出 1，因为 *pChar 访问的是一个 char 类型的数据


```

`int *pInt `表明这是一个指向整数的指针，当解引用 `*pInt`时，编译器知道这是个int类型数据，因此连续读取4个字节。



**2，指针算数运算** ：指针类型会影响算术运算的结果，指针进行加减运算的时候，**移动的单位是所指的数据类型的大小**

```c++
   int arr[5] = {1, 2, 3, 4, 5};
   int* pArr = arr;

   std::cout << *(pArr + 0) << std::endl; // 输出 1
   std::cout << *(pArr + 1) << std::endl; // 输出 2
   std::cout << *(pArr + 2) << std::endl; // 输出 3
```

​	pArr指向第一个元素。int类型数组，里面每个元素占4个字节。pArr作为指针+1的时候，回乡后移动sizeof(int)个字节.【就是四个字节】

**3，类型安全**：使用不同类型的指针可以提高代码的类型安全，编译器会根据类型进行类型检查，避免潜在错误.

```c++
   int num = 10;
   int* pNum = &num;
   char* pChar = reinterpret_cast<char*>(&num);



   *pNum = 20;           // 合法操作
   //*pChar = 'A';         // 错误：可能会覆盖掉 num 的一部分
```

  `*pNum = 20;   ` 是合法的。因为正在通过`int*`修改一个int类型的数据。

注释掉的是不合法的，它正试图通过一个`char*`修改int类型数据的一部分，这可能会破坏num完整性





## 野指针和悬垂指针有什么区别？

- 野指针：**是指没有被初始化或者初始化错误的指针。**这类指针指向一个不确定的内存地址，这个指针地址可能是随机值或者是无效的。
- 悬垂指针：指的是 **指向已经被释放内存的指针**。这种情况下指针仍持有旧地址，但是该地址的内存已被回收或者重新分配。

#### 产生原因：

**野指针产生原因：**声明后未初始化，或者初始化错误。

例如： int * p; cout<<*p<<endl; 没初始化的指针谁知道指向哪里去了。

**如何避免：**声明指针的时候，尽量将其初始化为nullptr.





**悬垂指针产生原因：** 对象或者内存被删除后，指针依然存在并指向已经释放的内存. 

例：使用delete或者delete[]操作符释放堆上分配的内存，但是指针没有被置为nullptr.

**如何避免：** 释放内存之后，及时将指针置为nullptr。



最好是使用智能指针来自动管理内存，减少直接操作裸指针

