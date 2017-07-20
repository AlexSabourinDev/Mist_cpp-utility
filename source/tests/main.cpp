#include "../../include/data-structures/RingBuffer.h"
#include "../../include/algorithms/Sorting.h"
#include "../../include/common/UtilityMacros.h"
#include "../../include/utility/BitManipulations.h"
#include "../../include/reflection/Type.h"
#include "../../include/reflection/Delegate.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <ctime>
#include <memory>

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

size_t ReturnNumber(size_t num) {
	return num;
}

void TestReflection() {

	std::cout << "Reflection Test" << std::endl;

	// -Type-

	Mist::Type type;
	type.DefineTypes<int>();

	MIST_ASSERT(type.Is<int>());
	MIST_ASSERT(type.Is<float>() == false);

	type.DefineTypes<float>();

	MIST_ASSERT(type.Is<int>() == false);
	MIST_ASSERT(type.Is<float>());

	std::unique_ptr<Mist::Detail::Callback> callback(Mist::Detail::MakeCallback(Pause));
	MIST_ASSERT(Mist::Detail::HasDefinition<void>(callback.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(callback.get())) == false);

	// -Callback-

	const size_t CHANGE_TARGET = 1;
	size_t changed;
	std::unique_ptr<Mist::Detail::Callback> lambda(Mist::Detail::MakeCallback([&changed, CHANGE_TARGET]() { changed = CHANGE_TARGET; }));

	MIST_ASSERT(Mist::Detail::HasDefinition<void>(lambda.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(lambda.get())) == false);

	Mist::Detail::CallbackInterface<void>* call = Mist::Detail::Cast<void>(lambda.get());
	call->Invoke();

	MIST_ASSERT(changed == CHANGE_TARGET);

	changed = 0;
	auto l = [&changed](size_t j) { changed = j; };
	std::unique_ptr<Mist::Detail::Callback> savedLambda(Mist::Detail::MakeCallback(l));

	MIST_ASSERT((Mist::Detail::HasDefinition<void, size_t>(savedLambda.get())));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(savedLambda.get())) == false);

	Mist::Detail::CallbackInterface<void, size_t>* c = Mist::Detail::Cast<void, size_t>(savedLambda.get());
	c->Invoke(CHANGE_TARGET);

	MIST_ASSERT(changed == CHANGE_TARGET);
	
	// -Delegate-

	int data = 0;
	Mist::Delegate del([&]() { data = CHANGE_TARGET; });
	del.Invoke();

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del2([](size_t j) { return j; });
	data = del2.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del3(&ReturnNumber);
	data = del3.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);


	std::cout << "Reflection Test Passed!" << std::endl;

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

	srand((unsigned int)time(0));

	// -Test-
	std::cout << "Sorting Test" << std::endl;

	std::cout << "Merge Sort" << std::endl;
	// Test a vector's sorting
	std::vector<size_t> m;
	
	// Test an array's sorting
	size_t arr[ELEMENT_COUNT];
	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT;
	}
	
	// Merge sort the array
	Mist::MergeSort(&arr[0], &arr[0] + ELEMENT_COUNT);
	MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));


	double totalSortTime = 0.0;
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		// Merge sort the vector
		Mist::MergeSort(&m);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));

	}
	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Quick Sort" << std::endl;

	
	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT;
	}
	
	// quick sort the vector
	Mist::QuickSort(std::begin(arr), std::end(arr));
	MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));
	
	totalSortTime = 0.0;
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// clear the vector and restart
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		// quick sort the vector
		Mist::QuickSort(&m);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));
	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Insertion Sort" << std::endl;

	totalSortTime = 0.0;
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// create a sorted vector of n elements
		std::vector<size_t> sortedVector;
		const size_t SORTED_ELEMENT_COUNT = ELEMENT_COUNT;
		for (int i = 0; i < SORTED_ELEMENT_COUNT; i++) {
			sortedVector.push_back(i);
		}

		// create a vector to insert into the vector
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		Mist::InsertionSort(m, &sortedVector);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(sortedVector), std::end(sortedVector)));

	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Sorting Tests Passed!" << std::endl;
}

void TestBitManipulations() {
	size_t mask = 0;
	// All the bits in the mask should be set, thus it's value should be max value
	MIST_ASSERT(Mist::SetLowerBitRange(0) == 0);
	MIST_ASSERT(Mist::SetLowerBitRange(1) == 1);
	MIST_ASSERT(Mist::SetUpperBitRange(sizeof(size_t) * 8) == std::numeric_limits<size_t>::max());
	MIST_ASSERT(Mist::SetBitRange(0, 2) == 3);
	MIST_ASSERT(Mist::SetBitRange(0, 3) == 7);
	MIST_ASSERT(Mist::SetBitRange(1, 3) == 6);
	MIST_ASSERT(Mist::CountBitsSet(mask) == 0);
	MIST_ASSERT(Mist::CountBitsSet(std::numeric_limits<size_t>::max()) == sizeof(size_t) * 8);
	
	size_t indices[] = { 0, 1 };
	MIST_ASSERT(Mist::GetBitMask(indices, 1) == 1);
	MIST_ASSERT(Mist::GetBitMask(indices, 2) == 3);
	MIST_ASSERT(Mist::GetBitFlag(1) == 2);

	mask = 0;
	MIST_ASSERT(Mist::SetBit(mask, 0) == 1);
	MIST_ASSERT(Mist::SetBit(mask, 1) == 2);
	mask = Mist::SetBit(mask, 0);
	MIST_ASSERT(Mist::SetBit(mask, 1) == 3);

	MIST_ASSERT(Mist::UnsetBit(mask, 0) == 0);
	MIST_ASSERT(Mist::ToggleBit(mask, 0) == 0);
	MIST_ASSERT(Mist::ToggleBit(mask, 1) == 3);

	MIST_ASSERT(Mist::IsBitSet(mask, 0) == true);
	MIST_ASSERT(Mist::IsBitSet(mask, 1) == false);

	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<size_t>::max(), 1, 3) == 6);
	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<size_t>::max(), 0, 2) == 3);

	size_t count;
	size_t ind[sizeof(size_t) * 8];
	Mist::GetIndividualBitIndices(mask, ind, &count);
	MIST_ASSERT(count == 1 && ind[0] == 0);
	Mist::GetIndividualBitIndices(3, ind, &count);
	MIST_ASSERT(count == 2 && ind[0] == 0 && ind[1] == 1);

	size_t masks[sizeof(size_t) * 8];
	Mist::GetIndividualBitFlags(mask, masks, &count);
	MIST_ASSERT(count == 1 && masks[0] == mask);

	Mist::GetIndividualBitFlags(3, masks, &count);
	MIST_ASSERT(count == 2 && masks[0] == 1 && masks[1] == 2);

	Mist::GetIndividualBitFlags(5, masks, &count);
	MIST_ASSERT(count == 2 && masks[0] == 1 && masks[1] == 4);

	MIST_ASSERT(Mist::GetMaskDifferences(3, 1) == 2);
	MIST_ASSERT(Mist::GetMaskDifferences(5, 3) == 2 + 4);
	MIST_ASSERT(Mist::GetMaskDifferences(8, 2) == 2 + 8);

}

int main() {

	TestRingBuffer();
	TestSorting();
	TestBitManipulations();
	TestReflection();

	Pause();
	return 0;
}
