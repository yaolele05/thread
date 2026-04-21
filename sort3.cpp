#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono> 
#include <queue>
#include <mutex>
#include <condition_variable>
using namespace std;

const int MIN_SIZE = 10000;

vector<int> generatedata(int n) {
    vector<int> data(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000000);
    for (int i = 0; i < n; i++) {
        data[i] = dist(gen);
    }
    return data;
}

void merge(vector<int>& a, vector<int>& t, int l, int mid, int r) {
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r) {
        if (a[i] < a[j]) 
        t[k++] = a[i++];
        else
         t[k++] = a[j++];
    }
    while (i <= mid)
    {
     t[k++] = a[i++];
    }
    while (j <= r)
    {
         t[k++] = a[j++];
    }
    for (int i = l; i <= r; i++) 
    {
        a[i] = t[i];
    }
}

void mergesort(vector<int>& a, vector<int>& t, int l, int r) {
    if (l >= r) return;
    int mid = (l + r) / 2;
    mergesort(a, t, l, mid);
    mergesort(a, t, mid + 1, r);
    merge(a, t, l, mid, r);
}

queue<pair<int, int>> tasks;  
mutex mtx;                    
condition_variable cv;        
bool done = false;
int livetasks = 0; 

void workerthread(vector<int>& a, vector<int>& temp) {
    while (true) {
        pair<int, int> task;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return !tasks.empty() || done; });

            if (done && tasks.empty()) return;

            task = tasks.front();
            tasks.pop();
            livetasks++;
        }

        int l = task.first;
        int r = task.second;

        if (r - l + 1 <= MIN_SIZE) {
            mergesort(a, temp, l, r);
        } else {
            int mid = (l + r) / 2;
            lock_guard<mutex> lock(mtx);
            tasks.push({l, mid});
            tasks.push({mid + 1, r});
        }

        {
            unique_lock<mutex> lock(mtx);
            livetasks--;
        }

        cv.notify_all();
    }
}

void famergesort(vector<int>& a, vector<int>& temp, int l, int r) 
{
    {
        lock_guard<mutex> lock(mtx);
        tasks.push({l, r});
    }
    cv.notify_all();

    vector<thread> threads;
    const int nuthreads = 4; 
    for (int i = 0; i < nuthreads; ++i) {
       threads.push_back(thread(workerthread, ref(a), ref(temp)));

    }
   
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return tasks.empty() && livetasks == 0; });
        done = true;
    }
    cv.notify_all();

    for (auto& t : threads) t.join();
}

int main() 
{
    int n = 1000000;
    vector<int> data = generatedata(n);
    vector<int> data2 = data;
    vector<int> temp(n);

    auto start2 = chrono::high_resolution_clock::now();
    famergesort(data2, temp, 0, n - 1);
    auto end2 = chrono::high_resolution_clock::now();

    cout << "多线程并发归并排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end2 - start2).count()
         << " ms" << endl;

    return 0;
}
