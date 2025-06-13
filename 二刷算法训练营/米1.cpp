#include <iostream>
#include<vector>
using namespace std;
bool judge(vector<int>&nums,int x,int y){
    int start=x;
    int end=y;
   // 循环判断是不是相等
   while(start<end){
        if(nums[start]==nums[end]){
            start++;
            end--;
        }else{
            return false;
        }
   }
   return true;
}
int main() {
   //检测回文子串，并且返回是回文子串的下标
int n;
cin>>n;
vector<int>str(n+1);//到时候根据下标输出相应的数就行
for(int i=1;i<=n;i++){
    cin>>str[i];
}

//然后执行回文串检测
//检测到两个相同元素之后将元素放进一个数组，然后该数组进行双指针判断是不是回文串，是则返回两个下标，不是则向下走
//对字符串遍历也是快慢指针吧，找到相同的字母之后是则返回，不是则慢指针进一个继续找
int slow=1;
vector<int>result;
for(int fast=slow;fast<=n;fast++){
    if(str[slow]==str[fast]&&judge(str,slow,fast)){
    //确实是回文串，就返回这两个下标，
    //然后从left右边继续开始检测
    result.push_back(slow);
    result.push_back(fast);
    slow=fast+1;
    fast=slow;
    }else{
      //  如果不是回文串slow++
      slow++;
    }

}

if(result[1]==0){
    cout<<"NO"<<endl;
}else{
    cout<<"YES"<<endl;
    for(int i=1;i<str.size();i++){
        cout<<str[i]<<" ";
    }
    cout<<endl;
}


}
// 64 位输出请用 printf("%lld")