#pragma once

#include "../common/UtilityMacros.h"
#include "../../include/utility/Hashing.h"
#include <cstdint>

MIST_NAMESPACE

// Maintains an abstraction between the maintanance of the hash value
// and the hash generation
class HashID {

public:

	// -Public API-

	inline uint64_t GetValue() const;
	

	// -Structors-

	inline constexpr HashID(const char* name);

	// Creates a hash id with a directly initialized value
	inline explicit HashID(uint64_t hashValue);

private:
	uint64_t m_HashValue = 0;

};


// -Implementation-

inline uint64_t HashID::GetValue() const {
	
	return m_HashValue;
}

inline constexpr HashID::HashID(const char* name) : m_HashValue(Hash64(name)) {}

inline HashID::HashID(uint64_t hashValue) : m_HashValue(hashValue) {}


MIST_NAMESPACE_END
