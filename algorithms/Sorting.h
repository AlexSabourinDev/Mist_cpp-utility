#pragma once

#include "../common/UtilityMacros.h"
#include <type_traits>
#include <iterator>
#include <utility>

// This folder implements a series of sorting algorithms useful for sorting different
// types of dat structures. Sorting functions that will be implemented are:
// - QuickSort
// - MergeSort
// - InsertionSort
// - HeapSort
// - BucketSort
// Possibly: Limited amount of memory sort, external sorting
// @Detail: Most of these implementations will use iterators in order to abstract the
//			use of accessing the elements. However, some versions of those algorithms will use
//			different types of arguments
MIST_NAMESPACE

namespace {
	// Internal method for minimum of two values
	template< typename MinType >
	MinType Min(MinType left, MinType right) {
		return left < right ? left : right;
	}
}



// -Merge Sort-

// The main implementation of merge sort will not be recursive, it uses O(n) extra memory
// the original collection is modified.
// @Detail: the implementation uses a swapping read and write buffers of size n and swaps between
//			them every change in block size.
template< typename CollectionType, typename IndexType = size_t, typename WorkingAreaType = CollectionType>
void MergeSort(CollectionType* collection, WorkingAreaType* workingArea = &CollectionType()) {

	const size_t collectionSize = std::size(*collection);
	size_t blockSize = 1;

	// Create our block iterators
	IndexType first, last, firstNext, lastNext;
	IndexType writeHead = 0;

	// Create our working area
	workingArea->resize(collectionSize);

	CollectionType* writeTarget = workingArea;
	CollectionType* readTarget = &*collection;

	// Keep going until we've passed the collection size for a block
	while (blockSize < collectionSize) {

		// Loop through all the block pairs in intervals of 2
		// Integer division will truncate which is expected behaviour
		size_t numBlocks = collectionSize / blockSize;
		for (size_t i = 0; i < numBlocks; i+=2) {
			// Select our first blocks
			first = i * blockSize;
			last = (i + 1) * blockSize;

			// Select our next blocks
			firstNext = (i + 1) * blockSize;
			// Assure that we don't go over the bounds of the collection
			lastNext = Min((i + 2) * blockSize, collectionSize);

			// Loop through both ranges and determine which part goes into the write first
			// keep going until we've written all of them
			while (firstNext != lastNext || first != last) {
				// if we've run out of the next block, write all the previous block
				if (firstNext == lastNext) {
					(*writeTarget)[writeHead++] = (*readTarget)[first++];
				}
				// if we've run out of the previous block, write all of the next block
				else if (first == last) {
					(*writeTarget)[writeHead++] = (*readTarget)[firstNext++];
				}
				// if the first is lower, that means we write that one and advance the write head
				else if ((*readTarget)[firstNext] < (*readTarget)[first]) {
					(*writeTarget)[writeHead++] = (*readTarget)[firstNext++];
				}
				else {
					(*writeTarget)[writeHead++] = (*readTarget)[first++];
				}
			}
		}

		// swap our read and write bodies and reset the write head
		std::swap(writeTarget, readTarget);
		writeHead = 0;

		// Increase our block size by the current blockSize
		blockSize += blockSize;
	}

	std::copy(std::begin(*readTarget), std::end(*readTarget), std::begin(*collection));
	return;
}


// The main implementation of merge sort will not be recursive, it uses O(n) extra memory
// the original collection is also modified. This is the version that sorts an array range
// @Detail: the implementation uses a swapping read and write buffers of size n and swaps between
//			them every change in block size.
template< typename ValueType, typename IndexType = size_t >
void MergeSort(ValueType* begin, ValueType* end) {

	const size_t collectionSize = end - begin;
	size_t blockSize = 1;

	// Create our block iterators
	IndexType first, last, firstNext, lastNext;
	IndexType writeHead = 0;

	// Create our working area, use a vector for the resource management and it's cleaner than std::unique_ptr<ValueType[]>
	std::vector<ValueType> workingArea(collectionSize);

	ValueType* writeTarget = workingArea.data();
	ValueType* readTarget = begin;

	// Keep going until we've passed the collection size for a block
	while (blockSize < collectionSize) {

		// Loop through all the block pairs in intervals of 2
		// Integer division will truncate which is expected behaviour
		size_t numBlocks = collectionSize / blockSize;
		for (size_t i = 0; i < numBlocks; i += 2) {
			// Select our first blocks
			first = i * blockSize;
			last = (i + 1) * blockSize;

			// Select our next blocks
			firstNext = (i + 1) * blockSize;
			// Assure that we don't go over the bounds of the collection
			lastNext = Min((i + 2) * blockSize, collectionSize);

			// Loop through both ranges and determine which part goes into the write first
			// keep going until we've written all of them
			while (firstNext != lastNext || first != last) {
				// if we've run out of the next block, write all the previous block
				if (firstNext == lastNext) {
					writeTarget[writeHead++] = readTarget[first++];
				}
				// if we've run out of the previous block, write all of the next block
				else if (first == last) {
					writeTarget[writeHead++] = readTarget[firstNext++];
				}
				// if the first is lower, that means we write that one and advance the write head
				else if (readTarget[firstNext] < readTarget[first]) {
					writeTarget[writeHead++] = readTarget[firstNext++];
				}
				else {
					writeTarget[writeHead++] = readTarget[first++];
				}
			}
		}

		// swap our read and write bodies and reset the write head
		std::swap(writeTarget, readTarget);
		writeHead = 0;

		// Increase our block size by the current blockSize
		blockSize += blockSize;
	}

	// Loop through the read target and write it to the passed in pointer
	for (int i = 0; i < collectionSize; i++) {
		begin[i] = readTarget[i];
	}
	return;
}



// -Quick Sort-

// Implementation:
// 


// -Utility Methods-

// Determine if a collection is sorted in O(n) time
template< typename IteratorType,
	// Template condition: Assure that the iterator value type is comparable
	// @Detail: This uses declval and decltype in order to test an expression and see if the return type is correct
	typename = std::enable_if<std::is_convertible<decltype(std::declval<typename std::iterator_traits<IteratorType>::value_type>() < std::declval<typename std::iterator_traits<IteratorType>::value_type>()), bool>::value>::type>
bool IsSorted(IteratorType begin, IteratorType end) {

	// Loop through all the elements and assure that the previous is lower than the next
	IteratorType next = begin + 1;
	for (; next != end; ++begin, ++next) {
		// If the next element is lower than the beginning one, the collection isn't sorted
		if (*next < *begin) {
			return false;
		}
	}
	return true;
}


MIST_NAMESPACE_END