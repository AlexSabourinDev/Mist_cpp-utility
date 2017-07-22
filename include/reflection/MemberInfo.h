#pragma once

#include "../common/UtilityMacros.h"
#include "MetaData.h"
#include "Delegate.h"

MIST_NAMESPACE

// Member info holds a reference to a member in a class allowing
// to be queried from any object
class MemberInfo {

public:

	// -Public API-

	template< typename MemberType, typename ClassType >
	MemberType* Get(ClassType* object);

	inline MetaData* GetMetaData();

	// -Types-

	template< typename ClassType, typename MemberType >
	using MemberPointer = MemberType ClassType::*;
	
	// -Structors-

	template< typename ClassType, typename MemberType >
	explicit MemberInfo(MemberPointer<ClassType, MemberType> memberPointer);

private:

	MetaData m_MetaData;
	Delegate m_MemberRetrieval;
};


// -Implementation-

template< typename MemberType, typename ClassType >
MemberType* MemberInfo::Get(ClassType* object) {

	return m_MemberRetrieval.Invoke<MemberType*, ClassType*>(object);
}

inline MetaData* MemberInfo::GetMetaData() {

	return &m_MetaData;
}

template< typename ClassType, typename MemberType >
MemberInfo::MemberInfo(MemberInfo::MemberPointer<ClassType, MemberType> memberPointer) 
	: m_MemberRetrieval([=](ClassType* object)->MemberType*{
	
		return &(object->*memberPointer);
	}) {}

MIST_NAMESPACE_END