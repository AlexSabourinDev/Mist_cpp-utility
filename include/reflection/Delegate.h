#pragma once

#include "../common/UtilityMacros.h"
#include "Type.h"
#include <tuple>
#include <memory>
#include <utility>

MIST_NAMESPACE

namespace Detail {

	// Determine the return type and argument types
	template< typename ObjectType, typename ReturnType, typename... ArgumentTypes >
	Type ExtractArgumentTypes(ReturnType (ObjectType::*method)(ArgumentTypes...)) {
		
		Type argumentTypes;
		argumentTypes.DefineTypes<ArgumentTypes...>();
		return std::move(argumentTypes);
	}

	// Const overload
	template< typename ObjectType, typename ReturnType, typename... ArgumentTypes >
	Type ExtractArgumentTypes(ReturnType (ObjectType::*method)(ArgumentTypes...) const) {
		
		Type argumentTypes;
		argumentTypes.DefineTypes<ArgumentTypes...>();
		return std::move(argumentTypes);
	}

	template< typename CallbackType,
		// @Template condition: This utilized SFINAE to remove this method from the
		// overload candidates when the callback type doesn't have operator()
		typename ReturnType = decltype(&CallbackType::operator()) >
	std::tuple<Type, Type> ExtractCallbackInfo(CallbackType callback) {

		Type returnType;
		returnType.DefineTypes<ReturnType>();

		return std::make_tuple(std::move(returnType), ExtractArgumentTypes(&callback::operator()));
	}

	template< typename ReturnType, typename... ArgumentTypes >
	std::tuple<Type, Type> ExtractCallbackInfo(ReturnType (*)(ArgumentTypes...)) {

		Type returnType;
		returnType.DefineTypes<ReturnType>();

		Type argumentTypes;
		argumentTypes.DefineTypes<ArgumentTypes...>();

		return std::make_tuple(std::move(returnType), std::move(argumentTypes));
	}

	class Callback {
		
	public:
		virtual ~Callback() = default;
	};


}


class Delegate {

public:

	// -Public API-

	// -Structors-
	template< typename CallbackType >
	Delegate();


private:

	Type m_ReturnType;
	Type m_ArgumentTypes;

	Detail::Callback m_Callback;
};

MIST_NAMESPACE_END