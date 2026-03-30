#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace chrono;

// 线程排序函数：给一段vector排序
void sort_part(vector<int>& arr, int left, int right) {
    sort(arr.begin() + left, arr.begin() + right);
}

int main() {
    cout << "=== C++ 多线程并行排序 ===" << endl;

    // 生成 100 万个随机数
    vector<int> arr(1000000);
    for (int& x : arr) x = rand() % 100000;

    auto start = high_resolution_clock::now();
    int mid = arr.size() / 2;

    // 两个线程分别排序前半、后半
    thread t1(sort_part, ref(arr), 0, mid);
    thread t2(sort_part, ref(arr), mid, arr.size());

    t1.join();
    t2.join();

    // 合并两个有序区间
    inplace_merge(arr.begin(), arr.begin() + mid, arr.end());

    auto end = high_resolution_clock::now();
    auto ms = duration_cast<milliseconds>(end - start).count();
    cout << "排序完成！总耗时：" << ms << "ms" << endl;
    return 0;
}