#pragma once

#include <utility>
#include "Macros.h"

MIST_NAMESPACE

// A simple ring buffer implementation with a read and write head. 
// Not atomic, but could easily be made atomic using atomic types
template< typename ValueType, size_t tSize >
class RingBuffer
{
	static_assert(tSize > 1, "A Ring Buffer Cannot be of size 0. Is it a typo?");

public:

	// -Types-
	using Type = ValueType;

public:

	// -Structors-
	template< typename... Args >
	RingBuffer(Args&&... defaultValueArguments) : m_Values(defaultValueArguments) {}

	RingBuffer() : m_Values() {}

public:

	// -Public API-

	// Attempts to read from the buffer, also consumes the read.
	// returns false if no value is available to read
	bool TryRead(ValueType* outValue);

	// Attempts to read from the buffer, does not consume the read.
	// returns false if no value is available to read
	bool TryPeek(ValueType* outValue);

	// Determine if there is any valid data for the user to read from the buffer
	inline bool CanRead();

	// Write a value into the buffer, moving the write head pointer forward.
	// If the method returns false, that means the next spot hasn't been read yet and nothing is written
	bool TryWrite(const ValueType& writeValue);

	// Write a value into the buffer, moving the write head pointer forward.
	// If the method returns false, that means the next spot hasn't been read yet and nothing is written
	// The moving version
	bool TryWrite(ValueType&& writeValue);

	// Determine if there is space to write to in the buffer
	inline bool CanWrite();

	inline size_t Size();

private:
	// Determines the previous region read in the buffer, it's always one step behind the next read
	signed int m_ReadHead = 0;
	// Determines the location that the next write will occure
	signed int m_WriteHead = 1;

	ValueType m_Values[tSize];
};


template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryRead(ValueType* outValue)
{
	if (CanRead() == false)
	{
		return false;
	}

	m_ReadHead = (m_ReadHead + 1) % tSize;
	*outValue = m_Values[m_ReadHead];
	return true;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryPeek(ValueType* outValue)
{
	if (CanRead() == false)
	{
		return false;
	}

	int readPosition = (m_ReadHead + 1) % tSize;
	*outValue = m_Values[readPosition];
	return true;
}

template< typename ValueType, size_t tSize >
inline bool RingBuffer<ValueType, tSize>::CanRead()
{
	// You can't read where hasn't been written yet
	int readLocation = (m_ReadHead + 1) % tSize;
	return readLocation != m_WriteHead;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryWrite(const ValueType& writeValue)
{
	if (CanWrite() == false)
	{
		return false;
	}

	m_Values[m_WriteHead] = writeValue;

	// Advance the write head
	m_WriteHead = (m_WriteHead + 1) % tSize;
	return true;
}

template< typename ValueType, size_t tSize >
bool RingBuffer<ValueType, tSize>::TryWrite(ValueType&& writeValue)
{
	if (CanWrite() == false)
	{
		return false;
	}

	// Make sure to keep the reference type instead of devolving into an lvalue
	m_Values[m_WriteHead] = std::forward<ValueType&&>(writeValue);

	// Advance the write head
	m_WriteHead = (m_WriteHead + 1) % tSize;
	return true;
}

template< typename ValueType, size_t tSize >
inline bool RingBuffer<ValueType, tSize>::CanWrite()
{
	// You can't write where was just read
	return (m_ReadHead == m_WriteHead) == false;
}

template< typename ValueType, size_t tSize >
inline size_t RingBuffer<ValueType, tSize>::Size()
{
	return tSize;
}

MIST_NAMESPACE_END