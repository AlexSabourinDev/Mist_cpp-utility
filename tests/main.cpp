
#include "../data-structures/RingBuffer.h"
#include "../algorithms/Sorting.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>

void Pause() {
	std::cout << "Press Any Key" << std::endl;
	char c;
	std::cin >> c;
}


void TestRingBuffer() {
	// -Test-
	std::cout << "RingBuffer Test" << std::endl;

	Mist::RingBuffer<size_t, 6> buffer;
	// Assure that the returned size is correct
	assert(buffer.Size() == 6);


	// Assure that can write and try write return the same value
	assert(buffer.CanWrite());
	// Assure that you can write into the circular buffer
	assert(buffer.TryWrite(10));
	// Assure that you can write a convertible type into the buffer
	assert(buffer.TryWrite(10l));
	

	for (size_t i = 0; i < 3; i++) {
		size_t lValue = 10;
		// Assure that can write and try write return the same value
		assert(buffer.CanWrite());
		// Assure that you can write into the circular buffer, assure that you can write lvalues
		assert(buffer.TryWrite(lValue));
	}

	for (size_t i = 0; i < 4; i++) {
		// Assure that can write and try write return the same value
		assert(buffer.CanWrite() == false);
		// Assure that we can't write when the buffer has been filled
		// The known amount you write is currently bound to be tSize - 1, 
		// this might be changed at a later time
		assert(buffer.TryWrite(10) == false);
	}

	size_t result = 0;
	for (size_t i = 0; i < 5; i++) {
		// Assure that can read implies the same as try read
		assert(buffer.CanRead());
		
		size_t otherResult = 0;
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
	Mist::RingBuffer<size_t, 10> exampleBuffer;
	size_t exampleResult = 0;
	if(exampleBuffer.TryWrite(20)) {
		assert(exampleBuffer.TryRead(&exampleResult));
		std::cout << exampleResult << std::endl;
	}
	if(exampleBuffer.TryRead(&exampleResult) == false) {
		std::cout << "Nothing left to read" << std::endl;
	}
}

void TestSorting() {
	// -Test-
	std::cout << "Sorting Test" << std::endl;

	// Test a vector's sorting
	std::vector<size_t> m;
	for (size_t i = 0; i < 33; i++) {
		m.push_back(rand() % 33);
	}

	// Merge sort the vector
	Mist::MergeSort(&m);
	assert(Mist::IsSorted(std::begin(m), std::end(m)));

	// Test an array's sorting
	size_t arr[33];
	for (size_t i = 0; i < 33; i++) {
		arr[i] = rand() % 33;
	}

	// Merge sort the array
	Mist::MergeSort(arr, arr + 33);
	assert(Mist::IsSorted(std::begin(arr), std::end(arr)));

	// Notify that the ring buffer tests have passed
	std::cout << "Sorting Tests Passed!" << std::endl;
}

int main() {
	TestRingBuffer();
	TestSorting();

	Pause();
	return 0;
}