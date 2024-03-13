#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>
#include <shared_mutex>

using namespace std;

#define N 1000 //size of array
#define M 10 //number of threads

void workingThread(int S[],vector<shared_mutex> &rwMutex,int ID){
    mt19937 generator(random_device{}());
    uniform_int_distribution<int> distribution(0, N-1);

    for(int it=0;it<10000;it++){
        int i = distribution(generator);
        int j = distribution(generator);

        int S_i0,S_i1,S_i2,S_j;
        {
            shared_lock<shared_mutex> lock0(rwMutex[i]);//Set read lock
            shared_lock<shared_mutex> lock1(rwMutex[(i+1)%N]);
            shared_lock<shared_mutex> lock2(rwMutex[(i+2)%N]);
            S_i0 = S[i];
            S_i1 = S[(i+1)%N];
            S_i2 = S[(i+2)%N];
        }

        S_j = S_i0 + S_i1 - S_i2;

        {
            unique_lock<shared_mutex> lock3(rwMutex[j]);//Set write lock
            S[j] = S_j;
        }
        printf("Thread number %d,Number of iterations:%d\n",ID,it);
    }
}

int main() {
    int S[N];
    vector<shared_mutex> rwMutex(N);
    vector<thread> threads;

    for(int i=0;i<M;i++){
        threads.emplace_back(workingThread,S,ref(rwMutex),i);
    }

    for(auto& thread:threads){
        thread.join();
    }

    return 0;
}
