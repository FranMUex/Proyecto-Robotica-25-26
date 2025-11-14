#include "ejemplo1.h"

int main(int argc, char** argv)
{
    ejemplo1 foo;
    std::this_thread::sleep_for(std::chrono::milliseconds(6900));
    return 0;
}
