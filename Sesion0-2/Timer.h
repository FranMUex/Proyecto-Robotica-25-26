#ifndef TIMER_H
#define TIMER_H
#include <thread>
#include <chrono>
#include <functional>
#include <future>
#include <cstdio>
#include <iostream>
#include <ctime>

class Timer
{
public:
    Timer(){};
    template <class T>
    void connect(T f)
    {
        std::thread([=]()
        {
            while(true)
            {
                  if(go.load())
                      std::invoke(f);
                  std::this_thread::sleep_for(std::chrono::milliseconds(period.load()));
            }
        }).detach();
    };
    void start(int p)
    {
        period = p;
        go = true;
    };
    void stop()
    {
        go = false;
    };
    void setInterval(int p)
    {
        period = p;
    }
private:
    std::atomic_bool go = false;
    std::atomic_int period = 0;
};
#endif