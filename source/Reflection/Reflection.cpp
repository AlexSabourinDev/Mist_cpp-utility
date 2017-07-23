#include "../../include/reflection/Reflection.h"
#include "../../include/common/UtilityMacros.h"

MIST_NAMESPACE

void Merge(Reflection&& left, Reflection* output) {

	MIST_ASSERT(output != nullptr);

#if MIST_DEBUG

	size_t typeCount = output->m_Types.size();
	size_t functionCount = output->m_GlobalFunctions.size();
	size_t objectCount = output->m_GlobalObjects.size();

#endif

	// merge the typeinfos
	for (auto& typeInfo : left.m_Types) {

		auto result = output->m_Types.emplace(typeInfo.first, std::move(typeInfo.second));
		MIST_ASSERT(result.second);
	}

	// merge the global functions
	for (auto& globalFunction : left.m_GlobalFunctions) {

		auto result = output->m_GlobalFunctions.emplace(globalFunction.first, std::move(globalFunction.second));
		MIST_ASSERT(result.second);
	}

	// merge the global objects
	for (auto& globalObjects : left.m_GlobalObjects) {

		auto result = output->m_GlobalObjects.emplace(globalObjects.first, std::move(globalObjects.second));
		MIST_ASSERT(result.second);
	}

#if MIST_DEBUG

	MIST_ASSERT(output->m_Types.size() == typeCount + left.m_Types.size());
	MIST_ASSERT(output->m_GlobalFunctions.size() == functionCount + left.m_GlobalFunctions.size());
	MIST_ASSERT(output->m_GlobalObjects.size() == objectCount + left.m_GlobalObjects.size());

#endif
}

MIST_NAMESPACE_END