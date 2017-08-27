#include "../../include/data-structures/RingBuffer.h"
#include "../../include/algorithms/Sorting.h"
#include "../../Mist_common/include/UtilityMacros.h"
#include "../../include/utility/BitManipulations.h"
#include "../../include/data-structures/SingleList.h"
#include "../../include/allocators/CppAllocator.h"
#include "../../include/data-structures/DynamicArray.h"

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





//void TestHash() {
//	
//	std::cout << "Hashing Test" << std::endl;
//	
//	std::vector<uint32_t> results;
//	results.push_back(Mist::djb2::Hash("lol"));
//	results.push_back(Mist::djb2::Hash("lol0"));
//	results.push_back(Mist::djb2::Hash("loldfsdaf"));
//	results.push_back(Mist::djb2::Hash("loldsafdsafdsafdsafdsafdsa"));
//	results.push_back(Mist::djb2::Hash("logfrwgvcxzgrl"));
//	results.push_back(Mist::djb2::Hash("lothrtjn xgtbtbreyl"));
//	results.push_back(Mist::djb2::Hash("logdsafhudesrv jklb l"));
//	results.push_back(Mist::djb2::Hash("lot5nbunel"));
//	results.push_back(Mist::djb2::Hash("lobtyrbtsbgtdsabdtl"));
//	results.push_back(Mist::djb2::Hash("loniuvgtehrgpb5gs8yniotbsrl"));
//	results.push_back(Mist::djb2::Hash("lobtrenbpvznurfbhobntrwgal"));
//	results.push_back(Mist::djb2::Hash("lolngjurenpwijnjivupr"));
//	results.push_back(Mist::djb2::Hash("looooofodfsaofdsafodasfodl"));
//	results.push_back(Mist::djb2::Hash("lonyetnusjrnfioNSfNOUfol"));
//	results.push_back(Mist::djb2::Hash("enbpvznurfbh"));
//	results.push_back(Mist::djb2::Hash("odfsaofdsafodasfodl"));
//	results.push_back(Mist::djb2::Hash("lonyetnusjrnfioN"));
//	results.push_back(Mist::djb2::Hash("loetnusupr"));
//	results.push_back(Mist::djb2::Hash("lngjureol"));
//	results.push_back(Mist::djb2::Hash("odfiukmmtnrhb"));
//	results.push_back(Mist::djb2::Hash("bytbdsfaazrs4b z"));
//	results.push_back(Mist::djb2::Hash("jkoytmnkodtynd"));
//	results.push_back(Mist::djb2::Hash("vrehivuorsabeFORNZeu i"));
//	results.push_back(Mist::djb2::Hash("foo"));
//	results.push_back(Mist::djb2::Hash("bar"));
//	results.push_back(Mist::djb2::Hash("vector"));
//	results.push_back(Mist::djb2::Hash("string"));
//	results.push_back(Mist::djb2::Hash("hash"));
//	results.push_back(Mist::djb2::Hash("m_hello"));
//	results.push_back(Mist::djb2::Hash("m_Lol"));
//	results.push_back(Mist::djb2::Hash("m_Hi"));
//	results.push_back(Mist::djb2::Hash("m_Value"));
//	results.push_back(Mist::djb2::Hash("m_Result"));
//	results.push_back(Mist::djb2::Hash("m_ShouldRun"));
//	results.push_back(Mist::djb2::Hash("m_IsActive"));
//	results.push_back(Mist::djb2::Hash("m_HasLife"));
//	results.push_back(Mist::djb2::Hash("m_ShouldBe"));
//	results.push_back(Mist::djb2::Hash("aaaaaaa"));
//	results.push_back(Mist::djb2::Hash("aaaa"));
//	results.push_back(Mist::djb2::Hash("aaaaaaaaaaa"));
//	results.push_back(Mist::djb2::Hash("aaaaaaaaa"));
//
//	
//	for(auto& i : results) {
//		for(auto& j : results) {
//			if(&i != &j)
//			{
//				MIST_ASSERT(i != j);
//			}
//		}
//	}
//	
//	std::cout << "Hashing Test Passed!" << std::endl;
//	
//}

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
		for (size_t i = 0; i < SORTED_ELEMENT_COUNT; i++) {
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

	std::cout << "Bucket Sort" << std::endl;



	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT + 10;
	}

	// quick sort the vector
	Mist::BucketSort(std::begin(arr), std::end(arr), (size_t)10, ELEMENT_COUNT + 10);
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
		Mist::BucketSort(&m, (size_t)0, ELEMENT_COUNT);
		totalSortTime += EndTimer();

		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));
	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Sorting Tests Passed!" << std::endl;
}

void TestBitManipulations() {
	Mist::BitField mask = 0;
	// All the bits in the mask should be set, thus it's value should be max value
	MIST_ASSERT(Mist::SetLowerBitRange(0) == 0);
	MIST_ASSERT(Mist::SetLowerBitRange(1) == 1);
	MIST_ASSERT(Mist::SetUpperBitRange(sizeof(Mist::BitField) * 8) == std::numeric_limits<Mist::BitField>::max());
	MIST_ASSERT(Mist::SetBitRange(0, 2) == 3);
	MIST_ASSERT(Mist::SetBitRange(0, 3) == 7);
	MIST_ASSERT(Mist::SetBitRange(1, 3) == 6);
	MIST_ASSERT(Mist::CountBitsSet(mask) == 0);
	MIST_ASSERT(Mist::CountBitsSet(std::numeric_limits<Mist::BitField>::max()) == sizeof(Mist::BitField) * 8);
	
	Mist::BitField indices[] = { 0, 1 };
	MIST_ASSERT(Mist::GetBitMask(&indices[0], 1) == 1);
	MIST_ASSERT(Mist::GetBitMask(&indices[0], 2) == 3);
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

	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<Mist::BitField>::max(), 1, 3) == 6);
	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<Mist::BitField>::max(), 0, 2) == 3);

	size_t count;
	Mist::BitField ind[sizeof(size_t) * 8];
	Mist::GetIndividualBitIndices(mask, ind, &count);
	MIST_ASSERT(count == 1 && ind[0] == 0);
	Mist::GetIndividualBitIndices(3, ind, &count);
	MIST_ASSERT(count == 2 && ind[0] == 0 && ind[1] == 1);

	Mist::BitField masks[sizeof(size_t) * 8];
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

void TestSingleList() {

	std::cout << "SingleList Tests" << std::endl;

	Mist::SingleList<size_t> list;
	
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsFirst(0);
	MIST_ASSERT(list.Size() == 1);
	MIST_ASSERT(list.RetrieveNodeAt(0) == list.FirstNode());
	MIST_ASSERT(list.RetrieveNodeAt(0) == list.LastNode());
	MIST_ASSERT(list.FirstNode() == list.LastNode());

	MIST_ASSERT(*list.RetrieveValueAt(0) == *list.FirstValue());
	MIST_ASSERT(*list.RetrieveValueAt(0) == *list.LastValue());
	MIST_ASSERT(*list.FirstValue() == *list.LastValue());

	list.Remove(list.FirstNode());
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsLast(2);
	MIST_ASSERT(*list.RetrieveValueAt(0) == 2);
	MIST_ASSERT(*list.FirstValue() == 2);
	MIST_ASSERT(*list.LastValue() == 2);

	list.InsertAsLast(10);
	MIST_ASSERT(*list.FirstValue() != *list.LastValue());
	MIST_ASSERT(list.FirstNode() != list.LastNode());
	MIST_ASSERT(*list.LastValue() == 10);
	MIST_ASSERT(*list.FirstValue() == 2);
	MIST_ASSERT(list.Size() == 2);

	list.InsertAfter(list.FirstNode(), 5);
	MIST_ASSERT(list.Size() == 3);
	MIST_ASSERT(*list.RetrieveValueAt(1) != *list.FirstValue());
	MIST_ASSERT(*list.RetrieveValueAt(1) != *list.LastValue());
	MIST_ASSERT(*list.RetrieveNodeAt(1)->GetValue() == *list.RetrieveValueAt(1));

	list.InsertAfter(list.LastNode(), 15);
	MIST_ASSERT(*list.LastValue() == 15);
	MIST_ASSERT(list.Size() == 4);
	
	list.InsertAsFirst(0);
	MIST_ASSERT(*list.FirstValue() == 0);
	MIST_ASSERT(list.Size() == 5);


	size_t previousValue = *list.FirstValue();
	Mist::SingleList<size_t>::Node* node = list.FirstNode();
	for (size_t i = 1; i < list.Size(); i++) {

		node = node->NextNode();
		MIST_ASSERT(*list.RetrieveValueAt(i) > previousValue);

		// Assure that the nodes match
		MIST_ASSERT(list.RetrieveNodeAt(i) == node);
	}

	list.InsertAfter(list.LastNode(), 20);
	MIST_ASSERT(*list.LastValue() == 20);

	MIST_ASSERT(list.Size() == 6);
	list.Remove(list.LastNode());
	MIST_ASSERT(list.Size() == 5);

	list.Clear();
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsFirst(0);
	list.Remove(list.FirstNode());
	MIST_ASSERT(list.Size() == 0);
	for (size_t i = 0; i < 10; i++) {

		list.InsertAsLast(i);
	}
	MIST_ASSERT(list.Size() == 10);
	list.Remove(list.FirstNode());
	MIST_ASSERT(*list.FirstValue() == 1);
	MIST_ASSERT(*list.LastValue() == 9);

	for (auto& i : list) {
		std::cout << *i.GetValue() << std::endl;
	}

	// Assure that moving works
	Mist::SingleList<size_t> newList(std::move(list));

	std::cout << "Single List Tests Passed" << std::endl;
}

void TestAllocator()
{
	std::cout << "Cpp Allocator Tests" << std::endl;

	size_t* pointer = Mist::CppAllocator::Alloc<size_t>(5);
	MIST_ASSERT(pointer != nullptr);
	Mist::CppAllocator::Free(pointer);

	void* block = Mist::CppAllocator::Alloc(100);

	*(size_t*)block = 10;
	MIST_ASSERT(block != nullptr);
	void* newBlock = Mist::CppAllocator::Realloc(block, 200);

	block = nullptr;

	MIST_ASSERT(newBlock != nullptr);
	// Assure that the memory got copied
	MIST_ASSERT(*(size_t*)newBlock == 10);

	void* newnewBlock = Mist::CppAllocator::Realloc(newBlock, 50);

	newBlock = nullptr;

	MIST_ASSERT(newnewBlock != nullptr);

	// Assure that the memory got copied
	MIST_ASSERT(*(size_t*)newnewBlock == 10);

	Mist::CppAllocator::Free(newnewBlock);
	
	std::cout << "Cpp Allocator Tests passed" << std::endl;
}

void TestDynamicArray() {

	std::cout << "Testing Dynamic Array" << std::endl;

	{
		Mist::DynamicArray<size_t> testArray;
		MIST_ASSERT(testArray.Size() == 0);

		testArray.Resize(10, 0);

		MIST_ASSERT(testArray.Size() == 10);

		for (auto i : testArray) {
			MIST_ASSERT(i == 0);
		}

		testArray.Clear();
		MIST_ASSERT(testArray.Size() == 0);

		testArray.InsertAsLast(10);
		MIST_ASSERT(testArray.Size() == 1);
		MIST_ASSERT(*testArray.LastValue() == 10);
		MIST_ASSERT(*testArray.FirstValue() == 10);
		MIST_ASSERT(*testArray.GetValue(0) == 10);
		MIST_ASSERT(testArray[0] == 10);

		testArray.RemoveLast();
		MIST_ASSERT(testArray.Size() == 0);

		testArray.Resize(10);
		for (size_t i = 0; i < 10; ++i) {
			MIST_ASSERT(*testArray.GetValue(i) == 0);
			MIST_ASSERT(testArray[i] == 0);
		}
	}

	{
		size_t constructionCount = 0;
		struct MemoryTracker {

			MemoryTracker(size_t* counter) {
				(*counter)++;
				c = counter;
			}

			MemoryTracker(const MemoryTracker& tracker) {
				c = tracker.c;
				(*c)++;
			}

			~MemoryTracker() {
				(*c)--;
			}

			size_t* c;
		};

		Mist::DynamicArray<MemoryTracker> testArray(10);
		MIST_ASSERT(testArray.ReservedSize() >= 10);

		// loop through the whole allocated raw array, even if the memory might be garbage set all the values to zero
		// this is to assure it doesnt crash
		for (size_t i = 0; i < testArray.ReservedSize(); i++) {
			memset(testArray.AsRawArray() + i, 0, sizeof(MemoryTracker));
		}

		testArray.InsertAsLast(MemoryTracker(&constructionCount));
		MIST_ASSERT(constructionCount == 1);

		testArray.Resize(10, &constructionCount);
		MIST_ASSERT(constructionCount == 10);

		MIST_ASSERT(testArray.Size() == 10);
		MIST_ASSERT(testArray.ReservedSize() <= testArray.Size());

		testArray.ReserveAdditional(10);
		MIST_ASSERT(testArray.ReservedSize() >= testArray.Size() + 10);
		testArray.ShrinkToSize();
		MIST_ASSERT(testArray.ReservedSize() == testArray.Size());

		Mist::DynamicArray<MemoryTracker> newTracker = std::move(testArray);

		MIST_ASSERT(constructionCount > 0);
		
		newTracker.Clear();
		MIST_ASSERT(constructionCount == 0);
	}



	std::cout << "Dynamic Array Tests Passed" << std::endl;
}

int main() {

	TestRingBuffer();
	TestSorting();
	TestBitManipulations();
	//TestReflection();
	//TestHash();
	TestSingleList();
	TestAllocator();
	TestDynamicArray();

	Pause();
	return 0;
}
