#pragma once

#include "../common/UtilityMacros.h"
#include <cstdint>

MIST_NAMESPACE

const uint64_t BEGIN_HASH_VALUE = 5381;
const uint64_t SHIFT_AMOUNT = 5;

// Hash a string at compile time, based on: https://stackoverflow.com/questions/7666509/hash-function-for-string
constexpr uint64_t Hash(const char* string, const uint64_t hash = BEGIN_HASH_VALUE) {
	return (string[0] == '\0') ? 0 : ((hash << SHIFT_AMOUNT) + hash) + string[1] + Hash(&string[1], hash);
}

MIST_NAMESPACE_END
