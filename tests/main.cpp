
#include "../include/RingBuffer.h"

#include <cassert>
#include <iostream>
#include <string>

void Pause()
{
	std::cout << "Press Any Key" << std::endl;
    char c;
    std::cin >> c;
}


void TestRingBuffer()
{
	std::cout << "RingBuffer Test" << std::endl;

	Mist::RingBuffer<int, 5> buffer;
	for (int i = 0; i < 5; i++)
	{
		assert(buffer.TryWrite(10));
	}
	for (int i = 0; i < 3; i++)
	{
		int result = 0;
		assert(buffer.TryRead(&result));

		std::cout << result << std::endl;
	}
}

int main()
{
	TestRingBuffer();

	Pause();
    return 0;
}