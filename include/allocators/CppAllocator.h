#pragma once

#include "../common/UtilityMacros.h"
#include <utility>
#include <type_traits>

#if MIST_DEBUG

// This define forces the cpp allocator to move the block of memory for realloc
// This is in case someone uses realloc and assumes it stays in place, this call forces it not to stay in place
#define MIST_USE_FORCED_MOVE_REALLOC 1

#endif

MIST_NAMESPACE

// The default allocator simply uses new and delete to implement it's functionality
class CppAllocator {
	
public:
	template< typename Type, typename... Arguments >
	static Type* Alloc(Arguments&&... args);

	static inline void* Alloc(size_t size);

	template< typename Type,
		// @Template condition, assure that you don't delete a void pointer,
		// only allow the void pointer version of free to be used
		typename TemplateCondition = typename std::enable_if<!std::is_same<void, Type>::value>::type >
	static void Free(Type* object);

	static inline void Free(void* block);

	// Reallocate a block of memory, this block might be at the same position or it might be at a different position
	// it's arbitrary to the OS, don't assume that thee block will stay at the same position.
	// newSize cannot be 0
	static inline void* Realloc(void* block, size_t newSize);
};


// -Implementation-

template< typename Type, typename... Arguments >
Type* CppAllocator::Alloc(Arguments&&... args) {

	Type* object = new Type(std::forward<Arguments...>(args...));
	MIST_ASSERT(object != nullptr);
	return object;
}

inline void* CppAllocator::Alloc(size_t size) {

	void* block = nullptr;

	MIST_ASSERT(size > 0);

	// Add the size of the block to the front of the allocation for extra information
#if MIST_DEBUG

	size_t* memBlock = (size_t*)malloc(sizeof(size_t) + size);
	MIST_ASSERT(memBlock != nullptr);
	// Log the information of the memblock at the beginning
	*memBlock = size;
	// Advance the pointer by one size_t
	block = (void*)(memBlock + 1);

#else

	block = malloc(size);

#endif

	return block;
}

template< typename Type, typename TemplateCondition>
void CppAllocator::Free(Type* object) {

	MIST_ASSERT(object != nullptr);
	delete object;
}

inline void CppAllocator::Free(void* object) {

	MIST_ASSERT(object != nullptr);

#if MIST_DEBUG

	size_t* sizePtr = (size_t*)object;
	// Return to the start of the original pointer, this is where we stored our size
	sizePtr = sizePtr - 1;
	// Return to the object pointer
	object = (void*)sizePtr;

#endif

	free(object);
}

inline void* CppAllocator::Realloc(void* oldBlock, size_t newSize) {

	MIST_ASSERT(newSize > 0);

	// Assure that the memory moves in order to avoid issues with assumptions that it won't move
#if MIST_USE_FORCED_MOVE_REALLOC && MIST_DEBUG

	// Allocate a new block of new size, assuring that the other block hasnt been freed yet
	void* newBlock = Alloc(newSize);

	size_t* oldBlockSizePtr = (size_t*)oldBlock;
	// Return to the original position of the size value
	oldBlockSizePtr = oldBlockSizePtr - 1;
	
	size_t oldSize = *oldBlockSizePtr;

	// Get the minimum size between the old size and the new size,
	// this assures that we don't copy too much information into the block
	size_t minSize = oldSize < newSize ? oldSize : newSize;

	// Copy the contents to a new block
	memcpy(newBlock, oldBlock, minSize);
	
	// set the old blocks memory to garbage
	memset(oldBlockSizePtr, 0xDB, sizeof(size_t) + oldSize);

	// Free the old block
	free(oldBlockSizePtr);

	return newBlock;

	// If we're not moving the block but still in debug, assure that the size is still set on this new pointer
#elif MIST_DEBUG

	// retrieve the original pointer
	size_t* oldBlockSizePtr = (size_t*)oldBlock;
	// Return to the original position of the size value
	oldBlockSizePtr = oldBlockSizePtr - 1;

	void* newBlock = realloc(oldBlockSizePtr, sizeof(size_t) + newSize);

	// If realloc fails, it doesn't free the old block of memory
	if (newBlock == nullptr) {
		free(oldBlockSizePtr);
		MIST_ASSERT(false);
	}

	// Set the size in the block
	size_t* newBlockSize = (size_t*)newBlock;
	*newBlockSize = newSize;

	// Advance the pointer to go passed the size
	newBlockSize = newBlockSize + 1;

	return (void*)newBlockSize;

#else

	// WARNING: If realloc fails, oldblock still exists!!
	return realloc(oldBlock, newSize);

#endif
}


MIST_NAMESPACE_END