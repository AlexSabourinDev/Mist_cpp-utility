
#include "../DataStructures/RingBuffer.h"

#include <cassert>
#include <iostream>
#include <string>

void Pause() {
	std::cout << "Press Any Key" << std::endl;
	char c;
	std::cin >> c;
}


void TestRingBuffer() {
	// -Test-
	std::cout << "RingBuffer Test" << std::endl;

	Mist::RingBuffer<int, 6> buffer;
	// Assure that the returned size is correct
	assert(buffer.Size() == 6);


	// Assure that can write and try write return the same value
	assert(buffer.CanWrite());
	// Assure that you can write into the circular buffer
	assert(buffer.TryWrite(10));
	// Assure that you can write a convertible type into the buffer
	assert(buffer.TryWrite(10l));
	

	for (int i = 0; i < 3; i++) {
		int lValue = 10;
		// Assure that can write and try write return the same value
		assert(buffer.CanWrite());
		// Assure that you can write into the circular buffer, assure that you can write lvalues
		assert(buffer.TryWrite(lValue));
	}

	for (int i = 0; i < 4; i++) {
		// Assure that can write and try write return the same value
		assert(buffer.CanWrite() == false);
		// Assure that we can't write when the buffer has been filled
		// The known amount you write is currently bound to be tSize - 1, 
		// this might be changed at a later time
		assert(buffer.TryWrite(10) == false);
	}

	int result = 0;
	for (int i = 0; i < 5; i++) {
		// Assure that can read implies the same as try read
		assert(buffer.CanRead());
		
		int otherResult = 0;
		// Assure that we can peek the value
		assert(buffer.TryPeek(&otherResult));
		// Attempt to read the amount written to the buffer
		assert(buffer.TryRead(&result));

		// Assure that the read and the peek returned the same value
		assert(otherResult == result);

		std::cout << result << std::endl;
	}

	// Assure that can read implies the same as try read
	assert(buffer.CanRead() == false);
	// Attempt to read more than was written, Assure that it returns false
	assert(buffer.TryRead(&result) == false);

	// Notify that the ring buffer tests have passed
	std::cout << "RingBuffer Tests Passed!" << std::endl;


	// -Example use-
	Mist::RingBuffer<int, 10> exampleBuffer;
	int exampleResult = 0;
	if(exampleBuffer.TryWrite(20)) {
		assert(exampleBuffer.TryRead(&exampleResult));
		std::cout << exampleResult << std::endl;
	}
	if(exampleBuffer.TryRead(&exampleResult) == false) {
		std::cout << "Nothing left to read" << std::endl;
	}
}

int main() {
	TestRingBuffer();

	Pause();
    return 0;
}