# vector

既然是动态数组，和静态数组的区别是什么？

为什么是动态数组呢？动态表现在哪里？

动态意味着其可以扩展数组的大小，动态扩容机制是什么？

扩容根据重置后的大小找一块内存空间，然后将之前的数据放在这块内存中。



说到STL，那就有一些通用的关键字：

begin 

end 

size：vector数组中元素的个数

 capaicity: vector的实际内存

erase删除vector中元素 ，参数为迭代器，删除一个元素之后，该元素后面的元素会向前一位，其他数据结构也有重载这个函数，但是性质不同的话，操作原理也不同，比如是链表的话后面的元素就不会都向前移动一位。

push_back向数组末尾放置元素。在其他位置构造之后，然后放入数组中时使用，或者是复制来的一个对象，然后放置在数组中

哎呦想起来还有个emplace_back,这个是直接在数组末尾构造一个元素是吧。是的，也是在末尾，记得如果初始化动态数组时候分配了一定的位置个数，则会在这几个后面进行添加新emplace_back的元素。

### algorithm中的函数find

**find 查找vector中元素位置** 嗯，这个我是真不会用，我以为直接"对象名.find()",这个参数有三个，起止位置以及要查找的元素信息，。返回的是个迭代器，指向查找到的第一个元素

find函数会借助 `==` 运算符来判定两个对象是否相等。

**在自定义对象使用STL容器的时候需要进行一些运算符重载。**

比如在我的代码中，我使用find函数发现并没有起作用。

```c++
#include <vector>
#include <iostream>
#include<string>
using namespace std;

struct dx1{
    int age;
    string name;
    dx1(){};
    dx1(int age,string name):age(age),name(name){cout<<"调用含参构造函数："<<name<<" | "<<age<<endl;};
};
int main(){
    vector<dx1>dxi;
    dxi.emplace_back(dx1(15,"ee"));
    dxi.emplace_back(dx1(16,"bb"));
    dxi.emplace_back(dx1(19,"cc"));

    for(auto i:dxi ){
        cout<<"age:"<<i.age<<"name:"<<i.name<<endl;
    }
    // vector<dx1>::iterator 
     auto it2 =find(dxi.begin(),dxi.end(),(16,"bb")); 报错！！原因--这是algorithm头文件中的函数
     //这边没办法找到与这个相同的元素，因为没有运算符重载，这是因为 find 函数会借助 == 运算符来判定两个对象是否相等。
    cout<<"find到的内容："<<it2->age<<it2->name<<endl;

     vector<dx1>::iterator  it3 =find(dxi.begin(),dxi.end(),(19,"bb"));
     cout<<"find到的内容："<<it3->age<<it3->name<<endl;
return 0;
}
```



#### 解决：

哈哈，find函数是algorithm中的函数，不是STL中的函数，为啥使用find函数报错，就是因为没有加上头文件，笑闷了。





移动构造和移动赋值运算符重载中

### 为什么要有`if(this!=&other)`

在移动赋值运算符重载函数中添加 `if (this != &other)` 条件判断主要是为了避免自我赋值（self-assignment）的情况。

#### 自我赋值导致的后果：

当发生自我赋值时，other和*this会指向同一个对象，然后执行移动语义的时候，std::move会将自身资源所有权转移走，并且清空other.name,也就是清空了this->name,这就导致了对象丢失原本持有的资源。最终结果就是该对象处于不合法的状态。



移动赋值运算符重载函数" = "几个容易遗忘的地方：

1. 返回值类型为引用类型，最后记得返回*this. 参数为右值，需要&&
2. 函数使用noexcept能够在使用容器时提高效率
3. 判断自赋值
4. 移动赋值时的操作。直接move

### 迭代器更新

迭代器相关知识，当容器内元素变化时，迭代器可能会失效然后重新指向新的位置，这种变化是怎么实现的呢？

维护指针。比如erase操作，删除一个元素之后，后面的元素同时向前一位，移动完成之后对指向末尾的指针进行操作，对指针执行自减操作，然后销毁最后一个元素（因为已经向前移动了）。



# part01:

### 二分法

```c++
class Solution {
public:
    int search(vector<int>& nums, int target) {
      int left=0;
      int right=nums.size()-1;//都是下标呗

      while(left<=right){//左闭右闭，middle的不同
        int middle=(left+right)/2;
            if(nums[middle]>target){//说明比target大，那就将右边的向左收紧
                right=middle-1;//为什么要减1，因为此时是大于Target的，当前这个nums[middle]一定不是target，那么接下来要查找的左区间结束下标位置就是 middle - 1
            }else if(nums[middle]<target){
                left=middle+1;
            }else{
                return middle;
            }
      }
    return -1;
    }

};

```



1. 重要的地方，每次循环的区间定义。也就是所说的“循环不变量”，这个“不变量”就是每轮循环的区间定义是不变的。也就是边界处理的时候，我们始终要根据这个不变的区间做边界处理。

2. 循环或者分支的进入条件是否满足，满足才会进入该循环或者该分支。

3. middle是否可以当做边界，当left<=right的时候，

   - 假如此时nums[middle]>target 也就是对right要赋值了
   - 1。 nums[middle]此时绝对不会等于target了
   - 2。 循环中会处理right==left的情况
   - 根绝这两个条件所以可以放心的将right=middle-1;  
   - 假如nums[middle]<target.  left要进行变换，那么left怎么变？
   - 1。nums[left]没有可能等于target了。
   - 2。nums[left]在循环中会处理和nums[right]相等的情况。
   - 根据这两个条件，left可以放心=middle+1;

   总结就是是否此时下标为middle的元素是否参与下次判断，若不参与且在下次循环中的处理会覆盖掉这个值相邻的元素，则可以放心的进行处理

也就是确保能够全覆盖所有可能得条件



----

二分法的核心要点在于维护 “循环不变量”，也就是每轮循环的区间定义保持不变，后续的边界处理都需依据这个不变的区间来操作。在使用二分法时，要关注循环或分支的进入条件，只有满足条件才会进入相应的循环或分支。

在`left <= right`的循环条件下，当计算出中间索引`middle`后，依据`nums[middle]`与目标值`target`的大小关系来调整边界：

- 若`nums[middle] > target`，由于`nums[middle]`肯定不等于`target`，且循环会处理`right == left`的情形，所以可以放心地将`right`更新为`middle - 1`。
- 若`nums[middle] < target`，鉴于`nums[left]`不可能等于`target`，并且循环会处理`nums[left]`和`nums[right]`相等的情况，所以能将`left`更新为`middle + 1`。

总体而言，关键在于判断下标为`middle`的元素是否会参与下一次判断。若不参与，且下次循环的处理会覆盖该元素相邻的元素，就能够安全地进行边界调整。





#### 也就是确保能够覆盖掉所有可能的条件？

可以这么理解。在二分法中维护 “循环不变量” 以及合理处理边界条件，本质上就是为了确保算法在执行过程中能够覆盖所有可能的情况。

通过每次循环对区间的合理收缩（根据中间元素与目标值的比较结果来调整左右边界），保证不会遗漏可能包含目标值的区域，从而在有限次循环后准确找到目标值（如果目标值存在于数组中）或者确定目标值不存在。



[27. 移除元素](https://leetcode.cn/problems/remove-element/)

```c++
class Solution {
    public:
    int removeElement(vector<int> &nums,int val){
        int slow=0;
        for(int fast=0;fast<nums.size();fast++){
            if(nums[fast]!=val){
                nums[slow++]=nums[fast];
            }
        }
        return slow;
    }
};
```

nums[fast]！=val的时候，下标fast对应的值赋给slow之后两个“指针”同时后移，当nums[fast]=val的时候，就slow不动，然后fast后移，在下次进入循环的时候，这个位置的值就会被slow路过的时候覆盖掉

[977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/)

思路：创建一个数组用于存放排序后的元素。原数组上首尾双指针，由于首尾向中间都属于递减，所以可以比较首尾平方后的数值，然后较大的放在新数组尾部。

```c++
class Solution {
public:
    vector<int> sortedSquares(vector<int>& nums) {
        int n=nums.size();
        vector<int>result(n);
        int slow=0;
        int fast=n-1;

        for(int i=nums.size()-1;i>=0;i--){
            int Fast2=nums[fast]*nums[fast];
            int Slow2=nums[slow]*nums[slow];
            if(Fast2>=Slow2){//此情况为右边大，那就右边的放进数组，fast指针左移
                result[i]=Fast2;//放入
                fast--;
            }else{
                result[i]=Slow2;//放入
                slow++;
            }
        }
        return result;
    }
};
第一次：双指针的话，从首尾各出发一个指针。然后将较大的放在result数组末尾，那单次较小的怎么办，放哪里？下次循环得出的值还要跟这个较小的值比吗？--这是指针的思想，但是又多了一点将两者完全取出来的思维，好像那个小的必须得进行处理才行，这种思维是错误的。

第2次：有思路了，直接比，比完一个右指针左移，左指针右移，然后将指的值直接放进result就行，回到上个问题，较小的怎么办？谁几把管你，你又没动，呆在那里等下次比较就行。
```

#### 处理双指针时的错误思想：

双指针的话，从首尾各出发一个指针。然后将较大的放在result数组末尾，那单次较小的怎么办，放哪里？下次循环得出的值还要跟这个较小的值比吗？--这是指针的思想，**但是又多了一点将两者完全取出来的思维**，好像那个小的必须得进行处理才行，这种思维是错误的。   -------- 说说我这种错误思想出现的原因，以及如何避免

1. 对双指针算法理解不够彻底

这种思想是没有完全正确理解双指针，但是理解了一点，因为你知道了要两处出发。但没有意识到，可以单独控制指针的移动。每次可以对一个指针所指元素进行处理，然后指针进行处理，也可以同时处理两个指针以及指向的元素。操作的灵活性很强的。

2. 受排序算法影响

常规排序算法通常是对数组中每个元素进行逐一比较和交换位置，这种思想让你习惯了对每个元素进行直接处理。但是**双指针中重要的思想是指针的移动操作**，而不是对每次指向的元素进行详尽处理。

3. 缺乏整体规划和动态视角

思考过程过度关注单个元素的处理，没有从整体考虑数组的有序性和双指针的动态移动过程。双指针算法是一个动态的过程，每次比较和指针移动都会影响后续操作，要从全局把握而不是孤立的看待每个元素。

#### 如何避免

深入理解算法原理。区分不同算法思维方式，就比如这次的双指针和排序一混就忘记了双指针的特性了。 建立算法的全局观

# part02

[209. 长度最小的子数组](https://leetcode.cn/problems/minimum-size-subarray-sum/)

```c++
class Solution {
public:
    int minSubArrayLen(int target, vector<int>& nums) {
        //首先看思路吧，一个循环，用于向后遍历，fast走，每次找到大于target的数之后与之前的长度进行比较，然后
        //更新slow的位置，循环更新
        int min_length=0;
        int result=INT_MAX;//最后返回的是这个
        int left=0;
        int sum=0;
        for(int right=0;right<nums.size();right++){
            sum+=nums[right];
            while(sum>=target){//也就是说只有满足条件，就进入循环，循环中处理什么呢、想想如果是if的话处理什么
                //会求长度. 妈的，处理left指针啊，滑动窗口处理左窗口的时机是在满足sum>=target的时候开始移动左窗口，我之前的思路：满足sum>=target之后，左窗口右移。
                min_length=right-left+1;//求长度 
                //左边处理,怎么搞呢? 
                sum-=nums[left++];
               result= min_length<result?min_length:result;//这里处理result小于minlength，是的话result=resut，第一次的话 一个最大的数小于0？显然不成立，所以result被赋值为计算出来的minlength
            }
        }
        return result==INT_MAX?0:result;//result为初始值的话说明根本没进入while(sum>=target)也就是根本不存在满足条件的子数组
    }
};


```

### 滑动窗口

此概念需要明确几个问题：

1. 窗口内是什么？
2. 如何移动窗口的起始位置
3. 如何移动窗口的终止位置

本题中对应这三个问题的答案：

- 窗口内是该窗口中元素大于等于target的连续子数组‘
- 当sum>=target的时候就该进行移动了，注意在右窗口不变的时候满足sum>=target可能左窗口移动多次依然满足，所以满足该条件的时候应该进入循环判断，也就是while（sum>=target）。而不是在右窗口不移动的情况下单次的if（sum>=target）。

- 终止位置（右窗口）就是遍历该数组嘛。也就是外层循环中的索引



#### 思路错误：

我的思路刚开始是对左窗口的移动有点误解的：

我理解的是当右窗口移动到一个位置之后左窗口**每次都要从起始位置**循环右移。这里每次从起始位置就是有些错误的。

误解2：不是很懂result的赋值操作，result应该被min_length赋值。直接result=minlength不就行了，嗯。。要和上次的result进行大小比较的。

如果计算的minlength比上个结果小才赋值给result

所以有result= minlength<result?minlength:result；

误解3：不是很懂为啥result要设置为    int result=INT_MAX； 看2就知道了，第一次算出来的minlength不论如何都比这个INT_MAX小。



螺旋矩阵

做法模拟，但是如何进行模拟很懵逼。代码使用什么结构进行处理方向的不同？怎么确定每个边填充的几次呢？比如n=4的时候与n=3的时候在圈层减1的时候每个边填充的元素都为1个，n=5的时候第二圈层是每次循环式填充两个。我草？所以每层填充几个是n/2啊？六层的时候，为3.

**嗯。。不对，思路不对，他的中心的那个是单独处理的，用loop代表处理几圈。5，4都是处理两圈，当其为5的时候有个心儿，心儿单独处理。也就是奇数的时候单独处理心儿**

https://kamacoder.com/problempage.php?pid=1070

### 前缀和

忘记了前缀和的概念了已经。

新创建一个数组，数组中每个元素存储的是该元素之前所有元素的和加上该元素的和。

求解区间内和的时候两种方法：

1. 传统暴力，将区间内值进行累加
2. 前缀和，根据计算好的数组，从区间尾对应的前缀和-（区间首下标-1）对应的前缀和



