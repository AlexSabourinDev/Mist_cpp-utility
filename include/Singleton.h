#pragma once

#include <cassert>
#include "Macros.h"

MIST_NAMESPACE

// The singleton class declares a static instance member for the declared type
// @Detail: the singleton count parameter provides the possibility of creating multiple singletons of the same type
template< typename SingletonType, int tSingletonCount = 1 >
class Singleton 
{
public:
    using Type = SingletonType;

    static constexpr int cSingletonCount = tSingletonCount;

    // @Detail: Declares a static variable within the instance method
	static SingletonType* Instance(unsigned int index = 0);

protected:
    Singleton() {};
};

// Implementation

// @Detail: Declares a static variable within the instance method
template< typename SingletonType, int tSingletonCount >
SingletonType* Singleton<SingletonType, tSingletonCount>::Instance(unsigned int index = 0)
{
	assert(index < tSingletonCount && "Index passed in is over the singleton count!");

	static SingletonType instance[tSingletonCount];
	return &instance[index];
}

MIST_NAMESPACE_END