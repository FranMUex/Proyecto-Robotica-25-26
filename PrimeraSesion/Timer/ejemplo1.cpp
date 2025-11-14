#include "ejemplo1.h"

#include <algorithm>
#include <pstl/parallel_backend_serial.h>

ejemplo1::ejemplo1()
{
	counter = 0;
	timer = new Timer();
	timer->connect(std::bind(&ejemplo1::increase_counter, this));
	timer->start(1000);
}

void ejemplo1::doButton()
{
	timer->stop();
}

void ejemplo1::increase_counter()
{
	counter++;
	std::cout << counter << std::endl;
}