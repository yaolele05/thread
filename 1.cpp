#include <vector>
#include <algorithm>
#include <thread>
#include <functional>
//生成随机数组
#include<bits/stdc++.h>
using namespace std;
vector<int> generate_random(int size)
{
    vector<int>arr(size);
    srand(time(0));
    for(int i=0;i<size;i++)
    {
        arr[i]=rand()%1000000;
    }
  return arr;
}
//单线程排序
void sort_sigle(vector<int>&arr)
{
    sort(arr.begin(),arr.end());
}
//数组分段（多线程灵魂）
//按CPU核心数分段

//多线程分段排序
void sort_r(vector<int>& arr,int l,int r)
{
    sort(arr.begin()+l,arr.begin()+r);
}
//多线程逻辑
void sort_m(vector<int>& arr)
{
    int cnum=thread::hardware_concurrency();
    int n=arr.size();
    int c=n/cnum;

    vector<thread> threads;
    //启动线程
    for(int i=0;i<cnum;i++)
    {
        int left=i*c;
        int right=(i==cnum-1)? n:(i+1)*c;//（i==cnum-1)就是最后一段万一出不尽，包括到最后
        threads.emplace_back(sort_r,ref(arr),left,right);//
    }
    //等待线程结束
    for(auto &t:threads)
    {
        t.join();
    }
    int step=c;
    while(step<n)
    {
       for(int i=0;i+step<n;i+=2*step)
       {
        int mid =i+step;
        int end=min(i+2*step,n);
        inplace_merge(arr.begin()+i,arr.begin()+mid,arr.begin()+end);
       }
       step*=2;
    }
}

/*
ref(arr)：必须传引用，否则拷贝巨慢
join()：等待线程完成，再去合并
无数据竞争！因为每个线程只写自己的区间
→ 这里不需要锁！（非常关键）
*/
//现在所有分段已经有序，需要多路并归
// 测试主函数
int main() {
    // 生成 100万 个随机数（数据越大，多线程优势越明显）
    int size = 1000000;
    vector<int> arr1 = generate_random(size);
    vector<int> arr2 = arr1; // 复制一份，保证两个排序用同样的数据

    // 测试单线程
    auto start = chrono::high_resolution_clock::now();
    sort_sigle(arr1);
    auto end = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "单线程排序时间：" << duration1 << "ms\n";

    // 测试多线程
    start = chrono::high_resolution_clock::now();
    sort_m(arr2);
    end = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "多线程排序时间：" << duration2 << "ms\n";

    // 验证结果是否正确（两个数组排序后必须完全一样）
    if (arr1 == arr2) {
        cout << " 排序结果正确！\n";
    } else {
        cout << " 排序结果错误！\n";
    }

    return 0;
}
