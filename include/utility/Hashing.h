#pragma once

#include "../common/UtilityMacros.h"
#include <cstdint>

MIST_NAMESPACE

namespace Detail {
	const uint32_t BEGIN_HASH_VALUE = 5381;
	const uint32_t HASH_SHIFT_AMOUNT = 5;
}

// Hash a string at compile time, Source on: https://stackoverflow.com/questions/7666509/hash-function-for-string
// Algorithm Source: http://www.cse.yorku.ca/~oz/hash.html by Dan Bernstein
// Special thanks to https://www.github.com/xoorath for pointing out that the method presented uses unsigned long which would use a 32 bit integer and not a 64 bit integer.
constexpr uint32_t Hash_djb2(const char* string, const uint32_t hash = Detail::BEGIN_HASH_VALUE) {
	return (string[0] == '\0') ? 0 : ((hash << Detail::HASH_SHIFT_AMOUNT) + hash) + string[1] + Hash_djb2(&string[1], hash);
}

MIST_NAMESPACE_END
