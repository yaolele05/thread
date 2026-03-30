//用锁解决竞态问题
#include <iostream>
#include <thread>
#include <mutex>
int count=0;
std::mutex mtx;
//线程执行函数
void add()
{
    for(int i=0;i<10000;i++)
    {
        //加锁
        mtx.lock();
        count++;
        //解锁
        mtx.unlock();
        /*
        std::lock_guard<std::mutex> lock(mtx);//自动加锁和解锁，
        count++;
        //不需要手动解锁，离开作用域会自动解锁
        */
    }
}
int main()
{
    //创建多个线程执行add函数,替代pthread——create
    std::thread t1(add);
    t1.join();
    std::thread t2(add);
    t2.join();

    std::cout<<count<<std::endl;//预期20000，但如果没有锁，可能小于20000
     return 0;
}