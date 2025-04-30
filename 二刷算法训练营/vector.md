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

# 数组

## part01:

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

## part02

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

-----

### 模拟法

螺旋矩阵

做法模拟，但是如何进行模拟很懵逼。代码使用什么结构进行处理方向的不同？怎么确定每个边填充的几次呢？比如n=4的时候与n=3的时候在圈层减1的时候每个边填充的元素都为1个，n=5的时候第二圈层是每次循环式填充两个。我草？所以每层填充几个是n/2啊？六层的时候，为3.

**嗯。。不对，思路不对，他的中心的那个是单独处理的，用loop代表处理几圈。5，4都是处理两圈，当其为5的时候有个心儿，心儿单独处理。也就是奇数的时候单独处理心儿**

#### 问题：

为什么使用while循环，该结构的特性是什么？什么时候循环结束呢？结束之后再进入循环条件是什么？

错误代码：

```c++
class Solution {
public://这里模拟的话，每圈分四次填充，填充的边界值记得整好，每次填充几个应该是n-1
    vector<vector<int>> generateMatrix(int n) {
        vector<vector<int>>result(n,vector<int>(n,0));//妈的为啥不初始化
        int loop=n/2;//就是需要处理几圈，n等于5的时候处理两圈，心儿单独处理，n=3的时候一圈，心儿单独处理
        int i=0,j=0;//进入下一圈的时候的得进行都加1处理,放在循环最后加吧
        int M_num=1;
        //四个墙
        int left=0;
        int right=n-1;
        int up=0;
        int down=n-1;
        while(loop--){//想想为什么要用while，以及什么时候会退出循环？

                //一圈的模拟处理.
                //左-》右
            for(;j<right;j++){
                result[i][j]=M_num;
                M_num++;
                right--;
            }
            //上-》下
            for(;i>down;i++){
                result[i][j]=M_num;
                M_num++;
                down--;
            }
            for(;j>left;j--){
                result[i][j]=M_num;
                M_num++;
                left++;
            }
            for(;i>up;i--){
                result[i][j]=M_num;
                M_num++;
                up++;
            }
            i++;
            j++;
        }
        if(n%2!=0){//有心的
            M_num++;
            result[i++][j++]=M_num;//
        }
        return result;
        
    }
};
```

#### 总结：

1. 重大错误，没有处理好不同对象的职责属性，比如i，j就是负责处理输出的result的中赋值用的，startX，startY就是为了处理矩阵用的，在之前的代码中，将i,j不仅处了轮旋矩阵还进行赋值操作，属于分工混乱了。**代码中创建的变量需要各司其职。**
2. 注意i j的巧妙使用，在单次循环中属于全局变量，累加起来之后还可以在下次for中使用，直接自减了。很好用的感觉。
3. 注意右->左 和 下->上 时候的终止条件，大于startX,startY 这个就属于矩阵用矩阵相关的变量了，他也会在一圈结束之后自加。 
4. 之前为了处理缩圈问题用了四个变量，在正确题解中只用了offset配合自加的startX与startY就完成了设置墙的操作。

```c++
class Solution {
public:
    vector<vector<int>> generateMatrix(int n) {
        vector<vector<int>>result(n,vector<int>(n,0));//创建一个矩阵用于返回结果
        int i,j;//o use set result[i][j];
        int loop=n/2;//to define how circle to process
        int startX=0;int startY=0;//to handle Matrix circle
        int offset=1;//处理一圈之后要缩圈
        int result_num=1;//填充的结果
        while(loop--){
            i=startX;
            j=startY;
            //因为要将处理后的矩阵的值赋值给result，所以将矩阵相关的startX给i
            for(;j<n-offset;j++){
                result[i][j]=result_num++;//赋值，自加
            }
            for(;i<n-offset;i++){
                result[i][j]=result_num++;
            }
            for(;j>startY;j--){//大于每次的起始位置，一圈结束之后startX会自加
            //并且注意，i,j在一次循环中属于全局变量，自加完之后，在单次while循环中下次使用依旧是自加之后的数值。
                result[i][j]=result_num++;
            }
            for(;i>startX;i--){//大于每次的起始位置，一圈结束之后startX会自加
                result[i][j]=result_num++;
            }
            startX++;
            startY++;
            offset++;
        }
        if(n%2!=0){
            result[n/2][n/2]=result_num;
        }
        return result;
    }
};
```

[LCR 146. 螺旋遍历二维数组](https://leetcode.cn/problems/shun-shi-zhen-da-yin-ju-zhen-lcof/)

```c++
class Solution {
public:
    vector<int> spiralArray(vector<vector<int>>& array) {
        int m=array.size();
        int n=m*m;
        vector<int>result(n,0);//输出结果，先初始化
        //依旧得设置每层的起始下标
        int startX=0;
        int startY=0;
        //缩圈的时候要范围减小，该值需加加
        int offset=1;
        //
        int i,j;//用于处理矩阵中的数字
        int loop=m/2;//这个是循环几圈
        int count=0;
        while(loop--){
            i=startX;
            j=startY;

            for(;j<m-offset;j++){
                result[count++]=array[i][j];
            }
             for(;i<m-offset;i++){
                result[count++]=array[i][j];
            }
             for(;j>startY;j--){
                result[count++]=array[i][j];
            }
             for(;i>startX;i--){
                result[count++]=array[i][j];
            }

            startX++;
            startY++;
            offset++;
        }
        if(n%2!=0){
            result[count]=array[m-1][m-1];
        }
        return result;
    }

};
```

#### 错误之处：

1. `n` 的计算有误

在代码里，`n` 被赋值为 `m * m`，但 `m` 是二维数组 `array` 的行数，`n` 正确的含义应该是二维数组中元素的总数。所以，`n` 应当是 `array` 的行数乘以列数，而不是 `m * m`。

2. 处理矩阵元素个数为奇数时的情况有问题

当矩阵元素个数为奇数时，代码直接把 `array[m - 1][m - 1]` 赋值给 `result[count]`，这是不对的。正确的做法是根据实际的循环状况来确定这个单独元素的位置。

3. 打印矩阵的时候用例矩阵可能是`1*3或者2*3这种非正方形矩阵`，所以在从左到右处理的时候应该是小于n-offset n是一维数组的大小

```c++
class Solution {
public:
    vector<int> spiralArray(vector<vector<int>>& array) {
        if (array.empty()) return {};
        int m=array.size();
        int n=array[0].size();
        
        vector<int>result(m*n,0);//输出结果，先初始化
        //依旧得设置每层的起始下标
        int startX=0;
        int startY=0;
        //缩圈的时候要范围减小，该值需加加
        int offset=1;
        //
        int i,j;//用于处理矩阵中的数字
        int loop=min(m,n)/2;//这个是循环几圈
        int count=0;
        while(loop--){
            i=startX;
            j=startY;

            for(;j<n-offset;j++){
                result[count++]=array[i][j];
            }
             for(;i<m-offset;i++){//注意一个是n-offset，一个是m-offset因为这里有可能不是正方形矩阵，可能是1*3的可能是2*3的矩阵
                result[count++]=array[i][j];
            }
             for(;j>startY;j--){
                result[count++]=array[i][j];
            }
             for(;i>startX;i--){
                result[count++]=array[i][j];
            }

            startX++;
            startY++;
            offset++;
        }
        // if(n%2!=0){
        //     result[count]=array[m/2][m/2];
        // }
     // 处理剩余元素
        if (min(m, n) % 2 == 1) {
            if (m <= n) {
                for (int k = startY; k < n - startY; k++) {
                    result[count++] = array[startX][k];
                }
            } else {
                for (int k = startX; k < m - startX; k++) {
                    result[count++] = array[k][startY];
                }
            }
        }

        return result;
    }

};
```



----

https://kamacoder.com/problempage.php?pid=1070

### 前缀和

忘记了前缀和的概念了已经。

新创建一个数组，数组中每个元素存储的是该元素之前所有元素的和加上该元素的和。

求解区间内和的时候两种方法：

1. 传统暴力，将区间内值进行累加
2. 前缀和，根据计算好的数组，从区间尾对应的前缀和-（区间首下标-1）对应的前缀和







# 链表

## part01

[203. 移除链表元素](https://leetcode.cn/problems/remove-linked-list-elements/)

有几个地方容易出错，并不是逻辑出错，而是指针方面可能会出错

1. 比如创建遍历指针的时候，创建`  ListNode *p=head->next;`还是创建`ListNode *p=head`? 

这里应该是创建后者，因为此时如果head为空的话，会引发空指针异常，因为对空指针 `head` 进行了 `->next` 操作。

2. 还有就是q的创建时机是什么时候呢？用的时候才创建，比如p->next->val==val的时候，才创建一个指针用于处理此情况。即用即创

```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* removeElements(ListNode* head, int val) {
        while(head!=nullptr&&head->val==val){
            //删除头结点，将下一个节点设置为头节点
        ListNode *temp=head->next;
        head->next=nullptr;
        head=temp;//重装上head就行
        }
        //处理head之后的节点
        ListNode *p=head->next;
         ListNode *q=p->next;
        while(p!=nullptr&&q!=nullptr){
            if(p->next->val==val){//找到相同的就删掉
                p->next=q->next;
                delete q;
              //  q=p->next;
            }else{
            p=p->next;
            }

        }
        return head;
    }   
};
```

正确代码：

```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* removeElements(ListNode* head, int val) {
        while(head!=nullptr&&head->val==val){
            //删除头结点，将下一个节点设置为头节点
        ListNode *temp=head->next;
        head->next=nullptr;
        head=temp;//重装上head就行
        }

下面注释的这段代码可以替换上面的处理头结点的代码，原理是一样的，写法不一样而已
        // while(head!=nullptr&&head->val==val){
        //     ListNode*temp=head;
        //     head=head->next;
        //     delete temp;
        // }
        //处理head之后的节点
        ListNode *p=head;
        while(p!=nullptr&&p->next!=nullptr){
            if(p->next->val==val){
                //找到相同的才创建临时变量
                ListNode *q=p->next;
                p->next=q->next;
                delete q;
            }else{
                p=p->next;//没找到就p后移
            }
        }
        return head;
    }   
};
```

充分理解在创建指针的时候避免空指针异常的情况。尤其是创建一个指针指向另一个指针的next的时候。





[707. 设计链表](https://leetcode.cn/problems/design-linked-list/)

嗯，一个链表类实现增删改查，我节点类/结构体写在哪里呢？链表类构造的时候无参含参怎么构造呢？

在链表类中创建链表节点结构体

```c++
class MyLinkedList {
public:
//节点结构体创建
    struct LinkedNode {
        int val;
        LinkedNode  *next;
        LinkedNode (int val):val(val),next(nullptr){};
    };
    MyLinkedList() {
        //链表类的话无参构造需要默认一个dummyhead其长度为空
        _size=0;
        _dummyhead=new LinkedNode (0);//指针参数，默认的话给你创建个新位置你指着吧
    }   
    
    int get(int index) {//头结点下标为0
        if(index<0||index>_size-1){
            return -1;
        }
        LinkedNode *cur=_dummyhead->next;
        while(index){
            cur=cur->next;
            index--;
        }//这个while循环的话，如果Index为0直接返回dummyhead->next 如果为其他的也可以正常返回，如果非正常输入的话，则直接返回-1了
        return cur->val;
    }
    //addAtHead应该是没问题的直接在虚拟头结点后面加了
    void addAtHead(int val) {
        LinkedNode  *Node=new LinkedNode (val);
        Node->next=_dummyhead->next;
        _dummyhead->next=Node;
        _size++;//节点加上了，链表长度加加
    }
    //加到尾节点应该直接将遍历的指针放到虚拟头结点比较好吧
    void addAtTail(int val) {
       // LinkedNode  *cur=_dummyhead->next;
        // while(cur->next!=nullptr){
        //     cur=cur->next;
        // }
        // LinkedNode *newNode=new LinkedNode (val);
        // cur->next=newNode;
        // _size++;
        LinkedNode  *cur=_dummyhead;
        while(cur->next!=nullptr){
            cur=cur->next;
        }
        LinkedNode *newNode=new LinkedNode (val);
        cur->next=newNode;
        _size++;
    }

    void addAtIndex(int index, int val) {//通过下标加也是，得将指针从头结点开始循环
        // LinkedNode *p=_dummyhead->next;
        // if(index>_size-1||index<0)return ;
        // if(index==_size-1){
        //     addAtTail(val);//如果等于长度，则加到末尾
        // }else{//小于总长度进行添加操作
        // index-=1;//减1，能够在p后面直接插一个节点
        //     while(index){
        //         p=p->next;
        //         index--;
        //     }
        //     LinkedNode  *newNode=new LinkedNode (val);
        //     newNode->next=p->next;
        //     p->next=newNode;
        //     _size++;
        // }
        //插入一般还是从虚拟头开始的。在第几个下标之前插入就意味着插入的这个下表为Index
        LinkedNode *cur=_dummyhead;
        if(index>_size)return;
        if(index<0)index=0;
        while(index--){
            cur=cur->next;
        }  
        LinkedNode*newNode=new LinkedNode(val);
        newNode->next=cur->next;
        cur->next=newNode;
        _size++;
 
    }
    
    void deleteAtIndex(int index) {
        LinkedNode *p=_dummyhead;
        if(index>_size-1||index<0)return ;
        while(index--){
            p=p->next;
        }
        LinkedNode *q=p->next;
        p->next=q->next;
        delete q;
        q=nullptr;
        _size--;
    }
private:
    int _size;
    LinkedNode  *_dummyhead;
     
};

/**
 * Your MyLinkedList object will be instantiated and called as such:
 * MyLinkedList* obj = new MyLinkedList();
 * int param_1 = obj->get(index);
 * obj->addAtHead(val);
 * obj->addAtTail(val);
 * obj->addAtIndex(index,val);
 * obj->deleteAtIndex(index);
 */
```

添加和删除的时候遍历指针最好从虚拟头结点开始，这样**处理头结点增删相关的操作时**会方便很多

在第一次处理的时候我是将循环指针设为LinkedNode *p=_dummyhead->next;。这样还要单独处理插入头结点的情况，也就是说设计虚拟头结点你却其二不用了，暴殄天物。





[206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/)

已经忘的一干二净了，我甚至想用一个dummyhead节点作为虚拟头节点，然后后面的逐个处理加在这个后面，但是题解根本没有使用虚拟头节点，而是使用pre指针和cur双指针。

```c++
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        //处理链表的时候要优先处理空链表的情况，因为和指针相关，所以就要注意空指针的情况。
        if(head==nullptr||head->next==nullptr)return nullptr;
        ListNode* _dummyHead=new ListNode(0);
        _dummyHead->next=head;
        ListNode*cur=_dummyHead;
        while(cur->next!=nullptr){

        }
};
```

双指针正解：

```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        //首先想想翻转链表需要什么东西进行翻转，指针应该怎么动，需要几个指针
        //想想一些特殊条件，头节点为空的情况。
        //if(head==nullptr&&head->next==nullptr)return head;
        ListNode *cur=head;//头节点上放个指针用于后移不犯毛病的
        ListNode *pre=nullptr;
        while(cur!=nullptr){//cur!=nullptr的时候继续循环，cur==nullptr的时候退出循环。为什么不是cur->next==nullptr的时候退出循环，因为cur->next==nullptr的时候还是得转换的呀，此时若是退出，则最后两个节点之间没有进行翻转
            ListNode*temp=cur->next;
            cur->next=pre;
            pre=cur;//pre先后移、、
            cur=temp;//cur再后移
        }
        return pre;
        }
};
```

二〇二五年四月三十日 这个**temp节点**没整明白其作用呀，他的作用是为了cur能够后移，而不是为了能够让pre连接到temp.今天又做了一遍总想让pre->next=temp.太极吧抽象了。

1.temp是为了给那个指针用的。。给cur后移使用的，因为的那个cur的呢next指针变的时候就没办法找到原本的后续节点了

2还有为什么先移动pre,后移动cur？因为如果先移动cur的话，cur变化了，pre就没办法移动了。

3为什么while循环的时候条件是cur！=nullptr。且返回的是pre？

-  cur!=nullptr的时候继续循环，cur==nullptr的时候退出循环。为什么不是cur->next==nullptr的时候退出循环，因为cur->next==nullptr的时候还是得转换的呀，此时若是退出，则最后两个节点之间没有进行翻转

4.初始化指针的时候怎么初始化，你是怎么想到的？



递归：

```c++
    /**
    * Definition for singly-linked list.
    * struct ListNode {
    *     int val;
    *     ListNode *next;
    *     ListNode() : val(0), next(nullptr) {}
    *     ListNode(int x) : val(x), next(nullptr) {}
    *     ListNode(int x, ListNode *next) : val(x), next(next) {}
    * };
    */
    class Solution {
    public:
        ListNode *reverse(ListNode* pre,ListNode* cur){
            if(cur==nullptr)return pre;
            ListNode*temp=cur->next;
            cur->next=pre;
            //单层循环结束之后进入下一层循环的做法：
          return  reverse(cur,temp);
        }
        ListNode* reverseList(ListNode* head) {
          return reverse(nullptr,head);
            }
    };
```

复习递归三部曲：

- 确定参数和返回值
- 确定终止条件
- 确定单层递归条件







## part02



















# 哈希表

## part01





整理下哈希增删改查：

key-value

**创建/构造**哈希map: `unordered_map<int,int>mapInt;`

**添加**map数据，三种添加方式：

`mapInt[1]=3;`

`mapInt.insert(pair<int,int>(1,3));`

`mapInt.insert(unordered_map<int,int>::value_type(1,3));`

**查找find()**:iterator find (const Key& k);  查找键`k`，若找到，返回指向该元素的迭代器；若未找到，返回`end()`迭代器。

find（）函数返回	的是一个迭代器指向的是键值为key的元素，find的参数是什么？如果没找到就返回指向map尾部的迭代器,也就是返回指向end()的迭代器。

**删除：**

erase();删除函数，有三种删除方法，按键删除、按迭代器删除以及范围删除



> 对stl的一些用法函数无非就是一些增删改查的使用，然后就是结合迭代器使用，几种STL的关键字基本都是通用的。

---

### [1. 两数之和](https://leetcode.cn/problems/two-sum/)

思路：

两数之和，使用哈希表怎么解？

一个整数数组，一个target目标值，从数组中找出两个数加起来和为target的两个元素下标。

遍历整个数组，遍历到一个插入到哈希表中一个。遍历到一个数的时候target-该数，能够在哈希表中找到，

但是怎么知道这个数对应的下标呢？使用`->first`访问吗？

但是这题中是将元素作为key,数组中的下标存储的时候作为Value，为什么呢？

这应该是跟哈希表的查找方式有关吧

map在此题中的作用：**存放已经遍历过的元素**，然后在后续遍历中查找target-num[i]是否能够在hash中找到，找到就返回存的value（也就是对应数组的下标），和此时遍历到的下标。



```c++
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int,int>toUse;//创建一个哈希表，助我使用
        vector<int>result(2,0);
        for(int i=0;i<nums.size();i++){
         //遍历，边遍历变插入？
         if(toUse.find(target-nums[i])==toUse.end()){
            toUse.insert(pair<int,int>(nums[i],i));
         }else{
            //如果找到了就返回两个下标
                result[0]=i;
                result[1]=toUse[target-nums[i]];
            return result;
         }   
        }
        return result;
    }
};   
```

可以变化的地方，find返回值为iterator迭代器，所以可以在 ==toUse.end（）的时候使用变量接收find的返回值

`auto it=toUse.find(target-nums[i]);`//嗯只是为了熟悉下auto和find的返回值的用法、

```c++
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int,int>TempHash;
        for(int i=0;i<nums.size();i++){
            auto it=TempHash.find(target-nums[i]);
            if(it==TempHash.end()){
                //没找到就放在hash中啊
              TempHash.insert(pair<int,int>(nums[i],i));  
            }else{
               // return {i,TempHash[target-nums[i]]}; 可以用，但是我已经有find到的元素的迭代器了，直接用下面的方法就行
                return {i,it->second};
            }
        }
        return {};
    }
};   
```



#### 四数相加



怎么解题？先两个数组进行相加，统计两个数组中出现的所有值以及这些值出现的次数。使用map存储，然后后两个数组从其中进行选择map中需要的数据，然后进行加加

```c++
class Solution {
public:
    int fourSumCount(vector<int>& nums1, vector<int>& nums2, vector<int>& nums3, vector<int>& nums4) {
        unordered_map<int,int>result;
        for(int a:nums1){
            for(int b:nums2){
               //可以用数组的形式代表哈希表
               result[a+b]++;
            }
        }
        int count=0;
        for(int c:nums3){
            for(int d:nums4){
                if(result.find(-(c+d))!=result.end()){
                    //代表能在result中找到对应的值，正因为需要和之前的内容相反，才需要取反之后再在hash中找对应键
                    count+=result[-(c+d)];//不能直接加加，因为有可能相同的结果出现好几次，所以应该是加result中的值
                }
            }
        }
        return count;
    }
};
```

- 若 `-(c + d)` 这个键存在，那么 `result[-(c + d)]` 就代表这个键对应的值，也就是 `nums1` 和 `nums2` 中元素相加结果为 `-(c + d)` 的组合的数量。把这个数量累加到 `count` 中，就可以得到满足 `nums1[i] + nums2[j] + nums3[k] + nums4[l] == 0` 的元组数量。

#### 快乐数

拆分为两个目标，目标1：拆分函数，每个多位数拆成单独的的数字。然后返回各个数字平方之后的和的结果。

目标2:将数据存入哈希表，循环判断其中有没有等于1的，有则return true。



```c#
class Solution {
public:
    int getNum(int n){
        int sum=0;
        while(n){
            sum+=(n%10)*(n%10);
            n=n/10;
        }
        return sum;
    }
    bool isHappy(int n) {
        unordered_set<int>result;
        while(1){
            int s=getNum(n);
            if(result.find(s)==result.end()){
                result.insert(s);
            }else if(s==1){
                return true;
            }else{
                //转回去循环调用getNum；怎么转回去？
                n=s;
            }
        }
    }
};
```

这里就陷入误区了。没明白循环中的处理信息，以及什么情况下处理什么。

#### 问题：

1. 不清晰什么时候返回false。

> ` if(result.find(s)!=result.end())`//哈希表中找到了和这次计算相同的数，意味着计算陷入循环了，之后的计算会不断重复这个循环，且这个循环不包含1因此不是快乐数

2. 刨除判断条件，也就意味着该循环一直处理的是：

```c++
while(1){
	s=getNum(n);
	n=s;
}
```

也就意味着一直循环，然后添加上条件，条件满足的时候才是退出循环的时机。

完整代码：

```c++
class Solution {
public:
    int getNum(int n){
        int sum=0;
        while(n){
            sum+=(n%10)*(n%10);
            n=n/10;
        }
        return sum;
    }
    bool isHappy(int n) {
        unordered_set<int>result;
        while(1){
            int s=getNum(n);
                if(result.find(s)!=result.end()){//哈希表中找到了和这次计算相同的数，意味着计算陷入循环了，之后的计算会不断重复这个循环，且这个循环不包含1因此不是快乐数
                    return false;
                }else if(s==1){
                    return true;    
                }else{
                    result.insert(s);
                }   
            n=s;
        }
    }
};
```

