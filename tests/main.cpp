
#include "../data-structures/RingBuffer.h"
#include "../algorithms/Sorting.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <ctime>

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
	const size_t SORTING_ITERATIONS = 1000;
	const size_t ELEMENT_COUNT = 200;

	srand(time(0));

	// -Test-
	std::cout << "Sorting Test" << std::endl;

	std::cout << "Merge Sort" << std::endl;
	// Test a vector's sorting
	std::vector<size_t> m;

	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		// Merge sort the vector
		Mist::MergeSort(&m);
		assert(Mist::IsSorted(std::begin(m), std::end(m)));

		// Test an array's sorting
		size_t arr[ELEMENT_COUNT];
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			arr[i] = rand() % ELEMENT_COUNT;
		}

		// Merge sort the array
		Mist::MergeSort(&arr[0], &arr[0] + ELEMENT_COUNT);
		assert(Mist::IsSorted(std::begin(arr), std::end(arr)));

	}

	std::cout << "Quick Sort" << std::endl;

	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// clear the vector and restart
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		// quick sort the vector
		Mist::QuickSort(&m);
		assert(Mist::IsSorted(std::begin(m), std::end(m)));

		// Test an array's sorting
		size_t arr[ELEMENT_COUNT];
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			arr[i] = rand() % ELEMENT_COUNT;
		}

		// quick sort the vector
		Mist::QuickSort(std::begin(arr), std::end(arr));
		assert(Mist::IsSorted(std::begin(arr), std::end(arr)));
	}

	// Notify that the ring buffer tests have passed
	std::cout << "Sorting Tests Passed!" << std::endl;
}

int main() {

	TestRingBuffer();
	TestSorting();

	Pause();
	return 0;
}