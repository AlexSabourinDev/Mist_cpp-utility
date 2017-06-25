#pragma once

#include "../common/UtilityMacros.h"
#include <type_traits>
#include <iterator>
#include <stack>
#include <utility>

// This folder implements a series of sorting algorithms useful for sorting different
// types of data structures. Sorting functions that will be implemented are:
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
	for (size_t i = 0; i < collectionSize; i++) {
		begin[i] = readTarget[i];
	}
}



// -Quick Sort-

// The main implementation of quick sort will be an in place quick sort. The implementation takes a
// begin and end iterator in order to sort the items in place.
template< typename IteratorType >
void QuickSort(IteratorType begin, IteratorType end) {

	// Implementation:
	// push a start and end range for the sort into a queue
	// Pick the first range of the queue
	// pick the last element of the range as the pivot
	// go through every item from the start to the end
	// keep in place if the item is greater than the pivot
	// swap with the first greater if the item is lower than the pivot if there is a first greater item
	// push the right range and the left range into the queue if their size is greater than one


	// Create the queue and push the intial range
	std::stack<std::pair<IteratorType, IteratorType>> sortingRanges;
	sortingRanges.push(std::make_pair(begin, end));

	// keep looping until the queue has been emptied
	while (sortingRanges.empty() == false) {
		// Get the most recent range and remove it
		std::pair<IteratorType, IteratorType> currentRange = sortingRanges.top();
		sortingRanges.pop();

		// Grab a pivot in the range from the back
		IteratorType pivot = currentRange.second - 1;
		IteratorType compareTarget = currentRange.first;
		
		// Initialize the first largest to the pivot to assure that we don't have a first largest as a smaller element
		IteratorType firstLargest = pivot;

		// Compare the target with the pivot
		// If the current target is smaller than the pivot, swap it with the first largest
		// If the current target is larger than the pivot, leave it in place
		for (; compareTarget != currentRange.second; ++compareTarget) {
			// Swap them if they aren't the same
			if (*compareTarget <= *pivot && firstLargest != pivot && compareTarget != pivot) {
				std::swap(*compareTarget, *firstLargest);
				// move the first largest forward since it is now smaller
				++firstLargest;
			}
			// If the compare target is larger than the pivot and we don't have a first largest yet,
			// track it as our first largest
			else if (*compareTarget > *pivot && firstLargest == pivot) {
				firstLargest = compareTarget;
			}
		}

		// Swap our values and set our new pivot position
		std::swap(*firstLargest, *pivot);
		pivot = firstLargest;

		// Once the pivots and the ranges have been created, add the new ranges to the queue
		
		// if the right range is larger than one, add it to the queue
		if (std::distance(pivot + 1, currentRange.second) > 1) {
			sortingRanges.push(std::make_pair(pivot + 1, currentRange.second));
		}

		// If the left range is larger than one, add it to the queue
		if (std::distance(currentRange.first, pivot) > 1) {
			sortingRanges.push(std::make_pair(currentRange.first, pivot));
		}
	}
}

// This version of quick sort simply pipes the results to the iterator version of quick sort.
// This version exists to match up with the merge sort interface.
template< typename CollectionType >
void QuickSort(CollectionType* collection) {
	QuickSort(collection->begin(), collection->end());
}


// -Utility Methods-

// Determine if a collection is sorted in O(n) time
template< typename IteratorType,
	// @Template condition: Assure that the iterator value type is comparable
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