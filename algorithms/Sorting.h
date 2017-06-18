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
// Possibly: Limited amount of memory sort
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

// The main implementation of merge sort will not be recursive
// it uses a random access iterator to create 4 range iterators that act as the merging groups
// for the algorithm.
template< typename IteratorType, 
	// Template condition: The iterator must be a random access iterator
	typename = std::enable_if<std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<IteratorType>::iterator_category>::value>::type,
	// Template condition: Assure that the iterator value type is comparable
	// @Detail: This uses declval and decltype in order to test an expression and see if the return type is correct
	typename = std::enable_if<std::is_convertible<decltype(std::declval<typename std::iterator_traits<IteratorType>::value_type>() < std::declval<typename std::iterator_traits<IteratorType>::value_type>()), bool>::value>::type>
void MergeSort(IteratorType begin, IteratorType end) {

	const size_t collectionSize = std::distance(begin, end);
	size_t blockSize = 1;

	// Create our block iterators
	IteratorType first, last, firstNext, lastNext;

	// Keep going until we've passed the collection size for a block
	while (blockSize < collectionSize) {

		// Loop through all the block pairs in intervals of 2
		// Integer division will truncate which is expected behaviour
		size_t numBlocks = collectionSize / blockSize;
		for (size_t i = 0; i < numBlocks; i+=2) {
			// Select our first blocks
			first = begin + i * blockSize;
			last = begin + (i + 1) * blockSize;

			// Select our next blocks
			firstNext = begin + (i + 1) * blockSize;
			// Assure that we don't go over the bounds of the collection
			lastNext = begin + Min((i + 2) * blockSize, collectionSize);

			// Merge both collections
			// loop through the next block and merge with the first block
			// we do the next block and the first block because we're using less than and
			// that requires us to move things left
			// Use preincrement to avoid an iterator copy
			for (; firstNext != lastNext; ++firstNext) {
				// If the lowest item in the next collection is lower than any item in the first collection swap it
				// and keep track of the currently changed item in the first collection
				// swap them and then move the first collection's iterator
				for (; first != last; ++first) {
					if (*firstNext < *first) {
						std::swap(*firstNext, *first);
						// We don't want to keep checking
						continue;
					}
				}
			}
		}

		// Increase our block size by the current blockSize
		blockSize += blockSize;
	}
}


// -Utility Methods-

// Determine if a collection is sorted in O(n) time
template< typename IteratorType,
	// Template condition: Assure that the iterator value type is comparable
	// @Detail: This uses declval and decltype in order to test an expression and see if the return type is correct
	typename = std::enable_if<std::is_convertible<decltype(std::declval<typename std::iterator_traits<IteratorType>::value_type>() < std::declval<typename std::iterator_traits<IteratorType>::value_type>()), bool>::value>::type>
bool IsSorted(IteratorType begin, IteratorType end) {

	// Loop through all the elements and assure that the previous is lower than the next
	IteratorType next = ++begin;
	for (; next != end; ++begin, ++next) {
		// If the next element is lower than the beginning one, the collection isn't sorted
		if (*next < *begin) {
			return false;
		}
	}
	return true;
}


MIST_NAMESPACE_END