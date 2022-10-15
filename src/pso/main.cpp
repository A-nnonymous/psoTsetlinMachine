#include <iostream>
#include "pso.h"
#include <mutex>
#include <vector>
#include <thread>


void addThread(int i)
{
    std::cout<<i<<"\n";
    while (true)
    {
        i ++;
        i --;
    }
    
}

int main()
{
    /*
    std::mutex mlock;
    std::vector<std::thread> vecth;
    for (size_t i = 0; i < 88; i++)
    {
        std::thread th(addThread, i);
        vecth.push_back(move(th));
    }
    for (size_t i = 0; i < 88; i++)
    {
        vecth[i].join();
    }
    //system("pause");
    */
   return 0;
}