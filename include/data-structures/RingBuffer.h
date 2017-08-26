#pragma once

#include <utility>
#include "../../Mist_common/include/common/UtilityMacros.h"

MIST_NAMESPACE

// A simple ring buffer implementation with a read and write head. 
// @Details: The size requested in the tSize parameter is not the guaranteed amount that you can write
//  in one go. It typically will be tSize - 1, but that might change in time.
// @Example: A contrived example use would look like: 
//
//		RingBuffer<int, 10> buffer;
//		int result = 0;
//		if(buffer.TryWrite(20)) {
//			assert(buffer.TryRead(&result));
//			std::cout << result << std::endl;
//		 }
//		if(buffer.TryRead(&result) == false) {
//			std::cout << "Nothing left to read" << std::endl;
//		}
template< typename ValueType, size_t tSize >
class RingBuffer {
	static_assert(tSize > 1, "A Ring Buffer Cannot be of size 0. Is it a typo?");

public:

	// -Public API-

	// Attempts to read from the buffer, also consumes the read.
	// returns false if no value is available to read
	bool TryRead(ValueType* outValue);

	// Attempts to read from the buffer, does not consume the read.
	// returns false if no value is available to read
	bool TryPeek(ValueType* outValue) const;

	// Determine if there is any valid data for the user to read from the buffer
	bool CanRead() const;

	// Write a value into the buffer, moving the write head pointer forward.
	// If the method returns false, that means the next spot hasn't been read yet and nothing is written
	template< typename WriteType = ValueType, 
		// @Template condition: the Writing type must be convertible to value type
		typename TemplateCondition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
	bool TryWrite(WriteType&& writeValue);

	// Determine if there is space to write to in the buffer
	bool CanWrite() const;

	size_t Size() const;


	// -Types-
	using Type = ValueType;


	// -Structors-
	RingBuffer() : m_Values() {}

private:
	// Determines the previous region read in the buffer, it's always one step behind the next read
	size_t m_ReadHead = 0;
	// Determines the location that the next write will occure
	size_t m_WriteHead = 1;

	ValueType m_Values[tSize];
};


// -Implementation-

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryRead(ValueType* outValue) {
	if (CanRead() == false) {
		return false;
	}

	m_ReadHead = (m_ReadHead + 1) % tSize;
	*outValue = m_Values[m_ReadHead];
	return true;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryPeek(ValueType* outValue) const {
	if (CanRead() == false) {
		return false;
	}

	size_t readPosition = (m_ReadHead + 1) % tSize;
	*outValue = m_Values[readPosition];
	return true;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::CanRead() const {
	// You can't read where hasn't been written yet
	size_t readLocation = (m_ReadHead + 1) % tSize;
	return readLocation != m_WriteHead;
}

template< typename ValueType, size_t tSize >
template< typename WriteType, 
	// @Template Condition: links to condition in class definition
	typename TemplateCondition >
bool RingBuffer<ValueType, tSize>::TryWrite(WriteType&& writeValue) {
	if (CanWrite() == false) {
		return false;
	}

	m_Values[m_WriteHead] = std::forward<WriteType&&>(writeValue);

	// Advance the write head
	m_WriteHead = (m_WriteHead + 1) % tSize;
	return true;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::CanWrite() const {
	// You can't write where was just read
	return (m_ReadHead == m_WriteHead) == false;
}

template< typename ValueType, size_t tSize >
size_t RingBuffer<ValueType, tSize>::Size() const {
	return tSize;
}

MIST_NAMESPACE_END
