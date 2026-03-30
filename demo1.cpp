#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
using namespace chrono;

// 模拟耗时任务
void task(const char* name) {
    auto start = high_resolution_clock::now();
    
    // 纯计算任务（多核会并行加速）
    for (int i = 0; i < 100000000; i++);
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "线程 " << name << " 完成，耗时：" << duration << "ms" << endl;
}

int main() {
    cout << "=== C++ 多线程：并发 vs 并行 ===" << endl;
    auto total_start = high_resolution_clock::now();

    // 创建两个线程
    thread t1(task, "A");
    thread t2(task, "B");

    t1.join();
    t2.join();

    auto total_end = high_resolution_clock::now();
    auto total = duration_cast<milliseconds>(total_end - total_start).count();
    cout << "\n总耗时：" << total << "ms" << endl;

    cout << "\n【结论】" << endl;
    cout << "单核CPU → 线程交替执行 = 并发（总耗时≈单线程2倍）" << endl;
    cout << "多核CPU → 线程真正同时跑 = 并行（总耗时≈单线程1倍）" << endl;
    return 0;
}