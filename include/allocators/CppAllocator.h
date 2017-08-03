#pragma once

#include "../common/UtilityMacros.h"
#include <utility>

MIST_NAMESPACE

// The default allocator simply uses new and delete to implement it's functionality
class CppAllocator {
	
public:
	template< typename Type, typename... Arguments >
	static Type* Alloc(Arguments&&... args);

	template< typename Type >
	static void Free(Type* object);
};


// -Implementation-

template< typename Type, typename... Arguments >
Type* CppAllocator::Alloc(Arguments&&... args) {

	return new Type(std::forward<Arguments...>(args...));
}

template< typename Type >
void CppAllocator::Free(Type* object) {

	delete object;
}


MIST_NAMESPACE_END