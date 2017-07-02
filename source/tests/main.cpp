#include "../../include/data-structures/RingBuffer.h"
#include "../../include/algorithms/Sorting.h"
#include "../../include/common/UtilityMacros.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <ctime>

// Simple timer methods
std::clock_t s_StartTime;

void BeginTimer() {
	s_StartTime = std::clock();
}

double EndTimer() {
	return (double)(std::clock() - s_StartTime) / (double)(CLOCKS_PER_SEC / 1000);
}

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
	MIST_ASSERT(buffer.Size() == 6);


	// Assure that can write and try write return the same value
	MIST_ASSERT(buffer.CanWrite());
	// Assure that you can write into the circular buffer
	MIST_ASSERT(buffer.TryWrite(0));
	// Assure that you can write a convertible type into the buffer
	MIST_ASSERT(buffer.TryWrite(10l));
	

	for (size_t i = 0; i < 3; i++) {
		size_t lValue = 10;
		// Assure that can write and try write return the same value
		MIST_ASSERT(buffer.CanWrite());
		// Assure that you can write into the circular buffer, assure that you can write lvalues
		MIST_ASSERT(buffer.TryWrite(lValue));
	}

	for (size_t i = 0; i < 4; i++) {
		// Assure that can write and try write return the same value
		MIST_ASSERT(buffer.CanWrite() == false);
		// Assure that we can't write when the buffer has been filled
		// The known amount you write is currently bound to be tSize - 1, 
		// this might be changed at a later time
		MIST_ASSERT(buffer.TryWrite(10) == false);
	}

	size_t result = 0;
	for (size_t i = 0; i < 5; i++) {
		// Assure that can read implies the same as try read
		MIST_ASSERT(buffer.CanRead());
		
		size_t otherResult = 0;
		// Assure that we can peek the value
		MIST_ASSERT(buffer.TryPeek(&otherResult));
		// Attempt to read the amount written to the buffer
		MIST_ASSERT(buffer.TryRead(&result));

		// Assure that the read and the peek returned the same value
		MIST_ASSERT(otherResult == result);

		std::cout << result << std::endl;
	}

	// Assure that can read implies the same as try read
	MIST_ASSERT(buffer.CanRead() == false);
	// Attempt to read more than was written, Assure that it returns false
	MIST_ASSERT(buffer.TryRead(&result) == false);

	// Notify that the ring buffer tests have passed
	std::cout << "RingBuffer Tests Passed!" << std::endl;


	// -Example use-
	Mist::RingBuffer<size_t, 10> exampleBuffer;
	size_t exampleResult = 0;
	if(exampleBuffer.TryWrite(20)) {
		MIST_ASSERT(exampleBuffer.TryRead(&exampleResult));
		std::cout << exampleResult << std::endl;
	}
	if(exampleBuffer.TryRead(&exampleResult) == false) {
		std::cout << "Nothing left to read" << std::endl;
	}
}

void TestSorting() {
	const size_t SORTING_ITERATIONS = 100;
	const size_t ELEMENT_COUNT = 100;

	srand((size_t)time(0));

	// -Test-
	std::cout << "Sorting Test" << std::endl;

	std::cout << "Merge Sort" << std::endl;
	// Test a vector's sorting
	std::vector<size_t> m;

	BeginTimer();
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		// Merge sort the vector
		Mist::MergeSort(&m);
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));

		// Test an array's sorting
		size_t arr[ELEMENT_COUNT];
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			arr[i] = rand() % ELEMENT_COUNT;
		}

		// Merge sort the array
		Mist::MergeSort(&arr[0], &arr[0] + ELEMENT_COUNT);
		MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));
	}
	std::cout << EndTimer() << "ms" << std::endl;

	std::cout << "Quick Sort" << std::endl;

	BeginTimer();
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// clear the vector and restart
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		// quick sort the vector
		Mist::QuickSort(&m);
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));

		// Test an array's sorting
		size_t arr[ELEMENT_COUNT];
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			arr[i] = rand() % ELEMENT_COUNT;
		}

		// quick sort the vector
		Mist::QuickSort(std::begin(arr), std::end(arr));
		MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));
	}

	std::cout << EndTimer() << "ms" << std::endl;

	std::cout << "Insertion Sort" << std::endl;

	BeginTimer();
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// create a sorted vector of n elements
		std::vector<size_t> sortedVector;
		const size_t SORTED_ELEMENT_COUNT = 10;
		for (int i = 0; i < SORTED_ELEMENT_COUNT; i++) {
			sortedVector.push_back(i);
		}

		// create a vector to insert into the vector
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		Mist::InsertionSort(m, &sortedVector);
		MIST_ASSERT(Mist::IsSorted(std::begin(sortedVector), std::end(sortedVector)));

	}

	std::cout << EndTimer() << "ms" << std::endl;

	std::cout << "Sorting Tests Passed!" << std::endl;
}

int main() {

	TestRingBuffer();
	TestSorting();

	Pause();
	return 0;
}