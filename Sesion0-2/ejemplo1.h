#ifndef ejemplo1_H
#define ejemplo1_H

#include "Timer.h"
#include <vector>

class ejemplo1
{
    public:
        std::vector<int> vec;
        int counter;
        Timer *timer;
        ejemplo1();
        void doButton();
        void increase_counter();
};

#endif // ejemplo1_H
