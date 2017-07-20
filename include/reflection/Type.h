#pragma once

#include "../common/UtilityMacros.h"
#include <memory>
#include <utility>

MIST_NAMESPACE

namespace Detail {

	// Define a base class for the type data in order to
	// hold it homogenously
	class TypeContainer {
	public:
		virtual ~TypeContainer() = default;
	};

	// Define the derived class that actually holds the type data
	template< typename... Types >
	class TypeDeclarations : public TypeContainer {};
}

class Type {

public:

	// -Public API-

	// Create the type and define it's core types
	// @Detail: Use an explicit method because a default templated constructor
	// cannot be invoked correctly as it believes its the type template list
	template< typename... CoreTypes >
	void DefineTypes();

	// Determine if this type is actually
	// the core types requested
	template< typename... CoreTypes >
	bool Is();

	// -Structors-

	Type() = default;
	~Type() = default;

	Type(Type&) = delete;
	Type& operator=(Type&) = delete;

	inline Type(Type&& move);
	inline Type& operator=(Type&& move);

private:
	
	std::unique_ptr<Detail::TypeContainer> m_Types;
};


// -Implementation-

template< typename... CoreTypes >
bool Type::Is() {
	// Please invoke Type::DefineTypes before calling Is
	MIST_ASSERT(m_Types != nullptr);

	Detail::TypeContainer* type = m_Types.get();
	// Try to dynamically cast it, if it doesn't return null, that means
	// it is the definition of those types
	return dynamic_cast<Detail::TypeDeclarations<CoreTypes...>*>(type) != nullptr;
}

template< typename... CoreTypes >
void Type::DefineTypes() {
	m_Types = std::make_unique<Detail::TypeDeclarations<CoreTypes...>>();
}

Type::Type(Type&& move) {
	m_Types = std::move(move.m_Types);
}

Type& Type::operator=(Type&& move) {
	m_Types = std::move(move.m_Types);
	return *this;
}

MIST_NAMESPACE_END