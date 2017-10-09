#pragma once

#include <Mist_Common/include/UtilityMacros.h>
#include "../allocators/CppAllocator.h"
#include <cstdint>
#include <type_traits>

MIST_NAMESPACE

template< typename ValueType, typename Allocator = CppAllocator >
class DynamicArray {

public:

	// -Public API-

	// Write a value into the array at the back
	template< typename... WriteType >
	void InsertAsLast(WriteType&&... writeValue);

	// Remove the last element of the array.
	// @Detail: the array will not shrink
	void RemoveLast();

	// Shrink the array to the desired size
	// @Detail: This shrinks the allocated memory to fit sizeof(ValueType) * Size() with no extra room
	void ShrinkToSize();

	// Resize the array to fit the desired size
	// @Detail: This might remove some elements from the array
	//  a size of zero is disallowed, call clear instead if you intend to empty the array
	template< typename... WriteValues >
	void Resize(size_t desiredSize, WriteValues&&... defaultValue);

	// This resizes the amount of allocated memory to fit the size specified
	// This is a good choice when you know how many items you're going to have as you'll avoid the
	// performance costs of multiple calls to the OS (If using the default allocator)
	// @Detail: The size cannot be zero as this doesn't make sense and should be checked out in the user side.
	//	if it is determined that 0 is the desired size, than remove the assert.
	void ReserveAdditional(size_t size);

	// @Detail: This method returns a reference instead of a pointer which varies from the rest of the API
	//  because I would rather stick to how an array would work than to the rest of the API.
	//  The other methods return a pointer in order to remain consisten with the rest of the API.
	ValueType& operator[](size_t index);

	ValueType* GetValue(size_t index);

	ValueType* FirstValue();

	ValueType* LastValue();

	ValueType* AsRawArray();

	size_t Size() const;

	size_t ReservedSize() const;

	// Remove the contents of the array, this completely removes
	// everything in the array and references to those items will be lost
	void Clear();

	// -Iterators-

	ValueType* begin();
	ValueType* end();

	// -Structors-

	DynamicArray() = default;
	// Create a dynamic array with the desired reserved space, you cannot invoke
	// operator[] as no elements were pushed into the array
	// @Detail: Internally, this just invokes ReserveAdditional.
	DynamicArray(size_t desiredReservedSpace);

	~DynamicArray();

	// Copying is currently disalllowed in the dynamic array, this is to avoid accidental copying. if it is desired,
	// an explicit copy method would be prefered, preferably outside this class in order to avoid
	// cluttering the api
	DynamicArray(const DynamicArray&) = delete;
	DynamicArray& operator=(const DynamicArray&) = delete;

	DynamicArray(DynamicArray&&);
	DynamicArray& operator=(DynamicArray&&);

private:

	static constexpr size_t GROWTH_BLOCK_SIZE = 5;

	void* m_Memory = nullptr;
	size_t m_ItemCount = 0;
	size_t m_MemorySize = 0;
};

// -Implementation-


template< typename ValueType, typename Allocator >
template< typename... WriteType >
void DynamicArray<ValueType, Allocator>::InsertAsLast(WriteType&&... writeValue) {

#if MIST_DEBUG
	size_t previousMemorySize = m_MemorySize;
#endif

	// Assure that we have enough space
	if (m_MemorySize < (m_ItemCount + 1) * sizeof(ValueType)) {
		ReserveAdditional(GROWTH_BLOCK_SIZE);
		
#if MIST_DEBUG
		/// Assure that the memory size actually grew after calling ReserveAdditional
		MIST_ASSERT(m_MemorySize > previousMemorySize);
#endif
	}

	// -Append the element to the back of the list-

	size_t nextItemOffset = m_ItemCount * sizeof(ValueType);
	// Cast the memory address to a size_t to do arithmetic on it
	// then add the offset to the address
	size_t nextItemPosition = ((size_t)m_Memory) + nextItemOffset;
	void* nextItemAddress = reinterpret_cast<void*>(nextItemPosition);

	// Use placement new to place a new valuetype at the correct position in the array
	ValueType* newItem = new (nextItemAddress) ValueType(std::forward<WriteType>(writeValue)...);
	MIST_ASSERT(newItem != nullptr);

	// Assure that we update our amount of items
	m_ItemCount++;
}

template< typename ValueType, typename Allocator >
void DynamicArray<ValueType, Allocator>::RemoveLast() {

	// Decrement our item count and call the destructor on the last item
	ValueType* lastItem = LastValue();
	lastItem->ValueType::~ValueType();

	m_ItemCount--;

#if MIST_DEBUG
	// Scramble the item to assure that it isn't reused and assure that we crash the program
	memset(lastItem, 0xDB, sizeof(ValueType));
#endif
}

template< typename ValueType, typename Allocator >
void DynamicArray<ValueType, Allocator>::ShrinkToSize() {

	// If we're already the right size, don't do anything, this is to avoid doing extra work
	if (m_MemorySize == m_ItemCount * sizeof(ValueType)) {
		return;
	}

	m_Memory = Allocator::template Realloc(m_Memory, m_ItemCount * sizeof(ValueType));
	m_MemorySize = m_ItemCount * sizeof(ValueType);
}

template< typename ValueType, typename Allocator >
template< typename... WriteValues >
void DynamicArray<ValueType, Allocator>::Resize(size_t desiredSize, WriteValues&&... defaultValues) {

	// Call Clear if you want to empty out the array
	MIST_ASSERT(desiredSize > 0);

	// If the new size is the same as the current size, don't do anything to avoid extra work
	if (m_ItemCount == desiredSize) {
		return;
	}
	// If the new size is larger than the current size, add elements until we've reached that size
	else if (desiredSize > m_ItemCount) {
		size_t newItemCount = desiredSize - m_ItemCount;
		for (size_t i = 0; i < newItemCount; ++i) {
			InsertAsLast(std::forward<WriteValues>(defaultValues)...);
		}
	}
	// If the new size is smaller than the current size, remove elements until we've reache that size
	// if (desiredSize < m_ItemCount)
	else {
		size_t removedItemCount = m_ItemCount - desiredSize;
		for (size_t i = 0; i < removedItemCount; ++i) {
			RemoveLast();
		}
	}

	MIST_ASSERT(m_ItemCount == desiredSize);
}

template< typename ValueType, typename Allocator >
void DynamicArray<ValueType, Allocator>::ReserveAdditional(size_t size) {

	// Assure that we reserve the byte memory, not just the count of items
	m_Memory = Allocator:: template Realloc(m_Memory, m_MemorySize + size * sizeof(ValueType));
	m_MemorySize += size * sizeof(ValueType);
}

template< typename ValueType, typename Allocator >
ValueType& DynamicArray<ValueType, Allocator>::operator[](size_t index) {

	return *GetValue(index);
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::GetValue(size_t index) {

	MIST_ASSERT(index < m_ItemCount);

	ValueType* values = reinterpret_cast<ValueType*>(m_Memory);
	return values + index;
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::FirstValue() {

	return GetValue(0);
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::LastValue() {

	return GetValue(m_ItemCount - 1);
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::AsRawArray() {

	return reinterpret_cast<ValueType*>(m_Memory);
}

template< typename ValueType, typename Allocator >
size_t DynamicArray<ValueType, Allocator>::Size() const {

	return m_ItemCount;
}

template< typename ValueType, typename Allocator >
size_t DynamicArray<ValueType, Allocator>::ReservedSize() const {

	return m_MemorySize / sizeof(ValueType);
}

template< typename ValueType, typename Allocator >
void DynamicArray<ValueType, Allocator>::Clear() {

	if (m_ItemCount == 0) {
		return;
	}

	// loop through and remove all the elements
	size_t arraySize = Size();
	for (size_t i = 0; i < arraySize; ++i) {
		RemoveLast();
	}

	Allocator:: template Free(m_Memory);
	m_Memory = nullptr;
	m_MemorySize = 0;

	MIST_ASSERT(m_ItemCount == 0);
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::begin() {

	return reinterpret_cast<ValueType*>(m_Memory);
}

template< typename ValueType, typename Allocator >
ValueType* DynamicArray<ValueType, Allocator>::end() {

	return reinterpret_cast<ValueType*>(m_Memory) + m_ItemCount;
}

template< typename ValueType, typename Allocator >
DynamicArray<ValueType, Allocator>::DynamicArray(size_t desiredReservedSpace) {

	ReserveAdditional(desiredReservedSpace);
}

template< typename ValueType, typename Allocator >
DynamicArray<ValueType, Allocator>::DynamicArray(DynamicArray&& rhs) {

	std::swap(m_Memory, rhs.m_Memory);
	std::swap(m_MemorySize, rhs.m_MemorySize);
	std::swap(m_ItemCount, rhs.m_ItemCount);
}

template< typename ValueType, typename Allocator >
DynamicArray<ValueType, Allocator>& DynamicArray<ValueType, Allocator>::operator=(DynamicArray&& rhs) {

	std::swap(m_Memory, rhs.m_Memory);
	std::swap(m_MemorySize, rhs.m_MemorySize);
	std::swap(m_ItemCount, rhs.m_ItemCount);

	return *this;
}

template< typename ValueType, typename Allocator >
DynamicArray<ValueType, Allocator>::~DynamicArray() {

	Clear();
}

MIST_NAMESPACE_END