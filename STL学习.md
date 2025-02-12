# unordered_map：

unordered_map和map类似，都是存储key_value的值，通过key快速索引到value。不同的是unordered_map不会根据key大小排序，存储时是根据key的hash值判断元素是否相同，即unordered_map内部元素是无序的。

unordered_map底层是一个防冗余的哈希表（**开链法**避免地址冲突）。

开链法：即当不同的键通过哈希函数映射到同一个位置时，这些键会被存储在一个链表中。

unordered_map的key需要定义hash_value函数并且重载operator==。

哈希表最大的特点就是把数据存储和查找消耗的时间大大降低，时间复杂度为O（1）；代价是消耗较多的内存。哈希表的查询时间虽然是O(1)，但是并不是unordered_map查询时间一定比map短，因为实际情况中还要考虑到数据量，而且unordered_map的hash函数的构造速度也没那么快，所以不能一概而论，应该具体情况具体分析。

## 负载因子：

定义为已使用的桶数/总桶数

负载因子是衡量哈希表的重要指标，它决定了哈希表的性能和效率。负载因子越高，哈希表的效率越低，因为更多的键被映射到一个桶中，导致更多的链表冲突和更高的查询时间。

## 重新哈希：

当已使用的桶数达到总桶数的某个阈值，也就是负载因子（通常为75%）容器就会自动扩容，这是为了保持性能，因为哈希表过于拥挤的时候，冲突的可能性会增加，导致查找、插入和删除操作的时间复杂度增加。扩容后，原有的键值对会被重新哈希到新的桶中，这个过程称为重新哈希。

重新哈希可能会影响性能，因为它需要额外的计算和内存分配。然而，如果不进行扩容，当负载因子超过 `0.75` 时，哈希表的性能可能会显著下降。



```c++
	unordered_map<string, int>dict;

	//直插
	dict.insert({ "nan",888 });
	dict.insert({ "nan",889 });//也没有覆盖，那和pair插入有什么区别呢

	//键值对插入
	dict.insert(pair<string, int>("qingf", 223));
	dict.insert(pair<string, int>("xiaoyu1", 224));
	dict.insert(pair<string, int>("xiaoyu1", 225));//相同的值并没有覆盖，刷新不了之前的值
	dict.insert(unordered_map<string, int>::value_type("ww", 22));
	dict.insert(unordered_map<string, int>::value_type("ss", 21));

	//赋值插入
	dict["banan"] = 225;
	dict["xiaoyu"] = 250;
	dict["xiaoyu"] = 520;	//这种插入会刷新值。本来是250，可以刷新为520

	//判断容器是否为空的函数
	if (dict.empty()) {
		cout << "无字符" << endl;
	}
	else {
		cout << "共有"<<dict.size()<<"个字符" << endl;
	}
	//size() is the unordered_map`s length

	//查找
	cout << dict.count("xiaoyu") << endl;
	//这个count 好像只返回 0 、1 0就是里面不包含这个键，1就是包含，而且里面没有重复的键

	//迭代器遍历!!!!!!!!
	unordered_map<string, int> ::iterator it;
	for (it = dict.begin(); it !=dict.end(); it++) {//byd, 用it<dict.size()还不行，只能用不等于end()
		cout << it->first << "--" << it->second << endl;
	}

	cout << endl;
	//使用auto mm接收返回的键值对
	auto mm=dict.find("xiaoyu");
	cout << mm->first;//输入键
	cout << mm->second;//输出值520；

	cout << endl;
	dict["xiaoyu"] = 5201314;//刷新xiaoyu的值

	//这里是用at访问键值对
	cout<<dict.at("xiaoyu1") << endl;//应该可以输出的224.确实是直接输出224.
	cout << "answer：224------------" << endl;

	//删除键为ss的键值对
	dict.erase("ss");

	//for each 遍历!!!!!!!!!
	for (const auto& pair : dict) {
		cout << pair.first << "---" << pair.second << endl;
	}
	cout << "------------" << endl;

	//dict.max_load_factor(0.80);//用不了，只能在自定义的哈希表中设置max_load_factor
	cout << dict.load_factor() << endl;

```

优雅的const  auto 和引用访问迭代器：

```c++
#include <unordered_map>
#include <iostream>

int main() {
    std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}, {3, "three"}};
    umap.erase(2);  // 删除键为 2 的元素
    for (const auto& pair : umap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}

```

### 什么时候用“.”什么时候用“->”

在 C++ 中，当你使用 `auto` 推断出一个对象类型时，如果这个对象是某个类的实例，你通常会使用 `.` 操作符来访问其成员。这是因为 `auto` 通常会推断出对象的具体类型，而 `.` 操作符用于访问该类型的成员。

与此相反，如果你使用 `auto` 推断出一个指针类型，那么你会使用 `->` 操作符来访问指针指向的对象的成员。这是因为 `->` 操作符用于访问通过指针**间接访问**的对象的成员。

### 上面优雅代码的使用const auto 和引用（&）

下面是 `auto`、`const` 和 `&` 通常在哪里使用以及它们的好处：

1. **auto**：
   - 用于自动推断变量类型，特别是在使用 STL 容器和迭代器时。
   - 提高代码的可读性，因为类型不需要显式指定。
   - 减少打字错误，因为类型由编译器自动推断。
2. **const**：
   - 用于声明常量，防止对变量进行意外修改。
   - 提高代码的安全性，因为编译器会检查常量引用和常量指针的使用。
   - 帮助其他开发者理解代码的意图，因为常量通常表示不可修改的数据。
3. **&**：
   - 用于创建引用，允许对原始变量进行操作。
   - 提高代码的性能，因为引用可以减少函数调用的开销。
   - 帮助其他开发者理解代码的意图，因为引用通常表示函数操作的是原始变量的副本。

在处理 STL 容器时，尤其是 `unordered_map`，`const auto&` 通常用于迭代器。例如，在遍历 `unordered_map` 时，你可以使用 `for (const auto& pair : umap)`。这种方式允许你安全地访问容器中的元素，同时确保元素不会被修改。

# map

## 为什么map底层用的是红黑树（RBT）而不是平衡二叉树（AVL）？

**平衡性:**红黑树是自平衡的二叉搜索树，他通过确保从根节点到叶子节点最长的可能路径不多于最短的可能路径的两倍长来保持树的平衡。这种平衡保证了红黑树的基本操作（如插，删，查）时间复杂度为O(logN).虽然AVL树也是平衡二叉树，但是要维护平衡的条件更严格，这导致在插删操作的时候需要更多的旋转操作来维持平衡。

**性能：**维护平衡的成本低，通过少量颜色变换和旋转即可保持平衡，而AVL需要更多的旋转操作。这使得插删操作下红黑树的性能更优

**实现复杂性：**红黑树的实现相对简单。	他只需要额外维护一个颜色信息，而二叉树需要维护每个节点的平衡因子，这增加了实现的复杂性。

**稳定性：**红黑树在插入和删除操作后，原有的元素顺序不会改变，这为map和Set提供了稳定的迭代顺序。而AVL在维护平衡的时候，可能改变元素顺序



突然明白了为什么说map是有序的，unordered_map是无序的，因为map是自平衡的二叉搜索树，所以他的中序遍历是有序的，而在map中，他对应的节点就是key_value的key，key必须是有序的，所以说map是有序的。而unordered_map是哈希表，所以没有顺序性可言。



## map和Set的区别，底层实现是什么？

都是C++容器类，底层实现都是红黑树（RB-Tree）

Set是const的，不允许修改元素的值；map是允许修改Value的，但是不允许修改key，如果真的要修改，就要将这个键删掉，然后调节平衡，再插入修改后的键值。如此一来，会导致导致与被删除节点相关联的迭代器会失效。这是因为删除操作会改变红黑树的结构，可能导致节点移动或者重新分配，从而使原有迭代器不再指向有效位置

除了被删除节点本身的迭代器失效，其他迭代器通常有效。原因是红黑树在删除节点的时候通常会保持其他节点相对位置不变，以维护整体的平衡性和有序性

在实际编程中，为了避免迭代器失效的问题，通常建议在修改容器（如插入或删除元素）之后，重新获取迭代器。

同时，使用`map`和`set`等容器时，应当注意它们提供的成员函数，如`erase`，这些函数会返回下一个有效迭代器，可以在删除操作后安全地继续使用。

如何使用：

```c++
map<int,string>::iterator it=myMap.begin();
while(it!=myMap.end()){
    if(someCondition(it->first)){
        it=myMap.erase(it);
    }else{
        ++it;
    }
}
```

这个函数中，使用while循环整个map，每次迭代检查当前元素是否满足someCondition函数。如果条件为真，我们调用erase方法删除函数**，并且更新迭代器it,为erase方法返回下一个有效迭代器。**如果不为真，我们通过++it迭代器后移，继续检查下一个元素，这样可以在遍历过程安全的删除元素，而不会导致迭代器失效。

## map中的key类型是对象或者是string类型的，他怎么进行排序呢？

1. 如果是内置类型的，(如整数、浮点数、字符等），`map`会使用这些类型的默认比较操作（通常是小于操作符`<`）来进行排序
2. 对于自定义对象作为键的情况，你需要为你的类重载小于操作符（`<`）。

map中的代码：

```c++
#include<iostream>
#include<map>
#include<string>
using namespace std;

class myObj{
public:
        int val;
        myObj(){};
        myObj(int v):val(v){};
        bool operator<(const myObj &obj)const{//重载小于操作符（`<`）
            return this->val<obj.val;
        }
};

int main(){
map<int,string>the_Map;

the_Map.insert({1,"xiaoyu,"});
the_Map.insert(pair<int,string>(2,"xiaoli"));
the_Map.insert({3,"loveyou"});
the_Map.insert({4,"forever"});

map<int,string>::iterator it;
it=the_Map.begin();

while(it!=the_Map.end()){
    cout<<it->second<<" ";
    ++it;
}
cout<<endl;

map<myObj,int>NM;
myObj myObj1(4);//栈上创建对象

// myObj s=new myObj();
//上面这句为什么错呢？因为要创建对象的话应该是将指针指向新创建的内存地址

myObj* tt=new myObj();//无参构造对象
myObj* yy=new myObj(6);//含参构造对象
NM.insert({*yy,8});

NM.insert({myObj(3),2});
NM.insert({myObj(1),2});
NM.insert({myObj(5),2});
NM.insert(pair<myObj,int>(myObj(2),5));
for(const auto &n: NM){
    cout<<"键："<<n.first.val<<"  值："<<n.second<<endl;
}
    return 0;
}
```



# vector

## vector的删除与三种遍历方式：

```c++
	vector<int>vec = { 11,2,3,4,5,6 };
	vector<int>::iterator i;//迭代器声明

	//删除
	vec.erase(vec.begin(), vec.begin() + 3);//删除，起始删除位置到终止删除位置。尽量不用，时间复杂度高
	vec.erase(vec.begin());

	//for each 遍历
	for (auto i : vec) {
		cout << i << endl;
	}	
	cout <<"----------" << endl;
	//迭代器遍历
	for (i = vec.begin(); i<vec.end(); i++) {
		cout << *i << endl;
        //这里是要用*访问其中的元素了，不能是vec.at(i)，也不是i->这样访问，因为这使用了迭代器
	}

	cout << "----------" << endl;
	//不使用迭代器的下标遍历
	for (int o = 0; o != vec.size(); o++) {
	//这里就不能用end（）和begin（）了，
		//因为这两个也是迭代器，表示起始和结尾用0和size（);
		cout << vec.at(o) << endl;
	}

```

因为vector是动态数组。删除其中一个元素的时候，会动态向前移动，一般不建议删除其中元素。

## vector二维数组

```c++
 //试着写一下吧 m层，每层有m-1个string字符串，并且初始化为hello
 int m
 vector<vector<string>>(m, vector<string>(m - 1, "hello"));
```

我注意到其中有两个string 就是第一个string 是为了声明vector中的元素类型，第二个string是为了初始化每一个元素的类型吗？

是的，您观察得很准确。在C++中，当我们使用`vector`的列表初始化语法时，`string`在代码中出现了两次，它们各自有不同的作用：

1. 第一个`string`：
   - `vector<string>`：这部分声明了内层`vector`中元素的类型。在这里，元素类型是`string`，所以每个内层`vector`都将包含`string`类型的对象。
2. 第二个`string`：
   - `"hello"`：这是一个字符串字面量，它用于初始化内层`vector`中的每个元素。当创建内层`vector`时，`(m - 1, "hello")`表示内层`vector`应该有`m - 1`个元素，并且每个元素都应该被初始化为字符串"hello"。

## 二维数组的遍历：

一种是用下标进行遍历，一种使用迭代器进行遍历：

```c++

//创建二维数组并进行初始化
vector<vector<int>> Snum(5,vector<int>(3,0));
	Snum[2][1]=2;
	Snum[2][2]=8;
	//使用双层循环的下标访问
	 for(int i=0;i<5;i++){
	 	for(int j=0;j<3;j++){
	 		cout<<Snum[i][j]<<" ";
		}
	 	cout<<endl;
	 }

	//使用迭代器遍历

	// vector<int>:: iterator it;
	// for (it=Snum.begin(); i!= ; i++)  失败，为什么，因为外层迭代器，也是双层定义的
	// {
	// 	/* code */
	// }
	vector<vector<int>>::iterator iter1;//外层
	vector<int>::iterator it;//内层迭代器。
	//用变量接收外层迭代的东西；
	vector<int> vec_temp;
	for(iter1=Snum.begin();iter1!=Snum.end();iter1++){
		//内层怎么写？ 需要创建一个vector暂存每次循环得到的vector数组，这里用vec_temp暂存；
		vec_temp=*iter1;
		for(it=vec_temp.begin();it!=vec_temp.end();it++){
			cout<<*it<<" ";
		}
		cout<<endl;
	}
```



当我尝试使用resize扩展空间的时候，会出现问题，

1，使用迭代器扩展的时候，会发生报错：

```c++

	//可以使用resize扩展列，我试一下
	//直接扩展列是不是不行，进循环拓展试试？
	for (iter1 = Snum.begin(); iter1 != Snum.end(); iter1++) {
		vec_temp = *iter1;
		vec_temp.resize(5);
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < vec_temp.size(); j++) {
			Snum[i][j] = (i + 1) * (j + 1);
			cout << Snum[i][j] << " ";
		}
		cout << endl;
	}

```

原因如下：不太清楚

但是使用数组下标扩展的时候不会出现报错：

```c++
for(int i=0;i<Snum.size();i++){
		Snum[i].resize(5);
	}
		for(int i=0;i<5;i++){
		for(int j=0;j<5;j++){
			Snum[i][j]=(i+1)*(j+1);
			cout<<Snum[i][j]<<" ";
		}
		cout<<endl;
	}
```



## 如何合理定制vector的内存空间

**.size()：** 代表查询它的动态数组vector中有多少个**有效数**

**.capacity():** 代表查询他的容量（最多放多少个数 / 即在不分配额外内存的情况下，vector可以存储的元素的最大数量）。

还有两个函数

**resize（Container::size_type n）**:强制把容器改为容纳n个元素。调用resize()之后，size会返回n，如果n小于当前大小，容器尾部元素将会被销毁。如果n大于当前大小，默认构造的元素将会添加到容器尾部。如果n大于当前容量，在元素添加之前会发生重新分配。

**reverse（Container::size_type n）**： 强n制容器将其容量改为至少为n，提供的n不小于当前大小。这一般强迫进行一次重新分配，因为容量需要增加（如果n小于当前容量，vector忽略它，这个调用什么都不做，string可能把它的容量减少为size()和n中大的数，但string的大小没有改变。

### 总结：

reverse：

1. 用于设置设置vector的capacity（即在不分配额外内存的情况下，vector可以存储的元素的最大数量）

2. 调用reverse后，如果指定的capacity大于当前的capacity，vector会重新分配内存以容纳新的capacity。

3. 如果reverse指定的capacity小于或等于当前的capacity，则vector的capacity不会改变

4. reverse不会改百年vector的size（即当前存储的元素数量）

resize：

1. 如果`resize`的参数小于当前的`size`，`vector`的`size`会减小到指定的值，并且会删除超出新`size`的元素。

2. 如果resize的参数大于当前size，vector的size会增加，并且新增加的元素会被默认构造（如果提供了额外的构造函数，则会使用该参数构造）
3. 如果resize的参数大于当前的capacity，则vector会重新分配内存以容纳新的size，从而增加capacity。如果参数小于或等于当前capacity，则不会改变capacity，只改变size。



## push_back和emplace_back 

push_back的话，如果我们添加的是自定义类，那么我们在使用的时候

```c++
vector<myClass>arr;
arr.push_back(myClass(1,"aheg") );
//这样添加到数组最后会发生什么?首先是在vector外面会构造出一个对象
//然后再move进数组中或者用通过拷贝构造进去。
//因为外面那个还在，所以将外面的删除

如果是emplace_back
arr.emplace_back(1,"second");
//其直接在vector内部构造出对象。对象位置就是在数组末尾

//然后在创建对象的时候，如果是左值引用的创建，在外层构造之后会有两步，一是将其拷贝到数组中，然后析构外面的那个对象，其是在函数周期结束后析构的，函数周期结束的时候，也会析构数组内那个对象。
//如果是右值引用的话，会在移动构造之后将其析构
```

![image-20240904155502213](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240904155502213.png)



我草，其他STL也有emplace ，emplace也是STL共性吗？emplace  安置，安放

# 左值和右值分别是什么东西、

## 左值：

**定义：**左值是一个具有持久生命周期的对象，有名字，可以在代码中多次引用

**例子：**变量名、数组名、指针、函数调用结果、

**行为：**左值可以出现在左边的表达式中、即可以被赋值。左值可以保持其引用，即使在函数中也可以保证其生命周期

## 右值：

**定义：**右值是具有临时生命周期的对象，没有名字，通常只在表达式中出现一次

**例子：**临时对象、字面量、函数返回值、表达式的一部分、

**行为：**右值通常不出现在左边的表达式中，即不能被赋值。它们通常与临时对象或函数返回值相关联，它们的生命周期仅限于表达式或函数调用。

C++中，左值和右值的概念与移动语义密切相关。移动语义允许程序员显式移动对象，而不是复制对象，这样可以提高性能。特别是移动大对象时。移动语句使用右值引用实现，这允许编译器区分左值和右值，并在必要时执行移动操作而不是复制操作。
