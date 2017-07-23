#pragma once

#include "../common/UtilityMacros.h"
#include "TypeInfo.h"
#include "Any.h"
#include "Delegate.h"
#include "../utility/HashID.h"
#include <unordered_map>

MIST_NAMESPACE

// Holds all of the reflection data such as:
// TypeInfos, GlobalFunctions, GlobalMethods
class Reflection {

public:

	// -Public API-

	template< typename ClassType >
	TypeInfo* AddType(HashID name);

	inline TypeInfo* GetType(HashID name);


	template< typename ReturnType, typename... Arguments >
	Delegate* AddGlobalFunction(HashID name, ReturnType (*function)(Arguments...));

	inline Delegate* GetGlobalFunction(HashID name);


	template< typename ObjectType >
	Any* AddGlobalObject(HashID name, ObjectType* object);

	inline Any* GetGlobalObject(HashID name);

	// -Global API-
	friend void Merge(Reflection&& left, Reflection* output);

private:
	std::unordered_map<uint64_t, TypeInfo> m_Types;
	std::unordered_map<uint64_t, Delegate> m_GlobalFunctions;
	std::unordered_map<uint64_t, Any> m_GlobalObjects;
};


// -Implementation-

template< typename ClassType >
TypeInfo* Reflection::AddType(HashID name) {

	Type classType;
	classType.DefineTypes<ClassType>();

	TypeInfo type(std::move(classType));

	auto result = m_Types.emplace(name.GetValue(), std::move(type));

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline TypeInfo* Reflection::GetType(HashID name) {

	return &m_Types.at(name.GetValue());
}


template< typename ReturnType, typename... Arguments >
Delegate* Reflection::AddGlobalFunction(HashID name, ReturnType(*function)(Arguments...)) {

	auto result = m_GlobalFunctions.emplace(name.GetValue(), function);

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline Delegate* Reflection::GetGlobalFunction(HashID name) {

	return &m_GlobalFunctions.at(name.GetValue());
}


template< typename ObjectType >
Any* Reflection::AddGlobalObject(HashID name, ObjectType* object) {

	auto result = m_GlobalObjects.emplace(name.GetValue(), object);

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline Any* Reflection::GetGlobalObject(HashID name) {

	return &m_GlobalObjects.at(name.GetValue());
}

MIST_NAMESPACE_END