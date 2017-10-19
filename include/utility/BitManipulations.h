#pragma once

#include <Mist_Common\include\UtilityMacros.h>
#include <cstdint>

MIST_NAMESPACE

// -Public API-

using BitField = uint32_t;

// Determine if a bit is set
inline bool IsBitSet(const BitField mask, const BitField index);

// Set a bit to on
// the index must be less than sizeof(BitField) * 8
inline BitField SetBit(const BitField mask, const BitField index);

// Toggle a bit from on to off or off to on
inline BitField ToggleBit(BitField mask, const BitField index);

// Set a bit to off
inline BitField UnsetBit(const BitField mask, const BitField index);

// Determine if a flag is set inside of the mask
inline bool IsFlagSet(const BitField mask, const BitField flag);

// Determine how many flags are set in the mask
inline size_t CountBitsSet(BitField bits);

// Get all of the set flags in the mask as their own masks
inline void GetIndividualBitFlags(BitField mask, BitField* bits, size_t* maskCount);

// Get all of the indices of the bits set in the mask
inline void GetIndividualBitIndices(const BitField mask, BitField* bitIndices, size_t* indexCount);

// Get a bit mask of all the bit indices
inline BitField GetBitMask(const BitField* bitIndices, const size_t indexCount);

// Get a bit mask for the bit passed in
inline BitField GetBitFlag(const BitField bitIndex);

// Set all the bits from the range begin to end (exclusive)
inline BitField SetBitRange(const BitField begin, const BitField end);

inline BitField GetBitRange(const BitField mask, const BitField begin, const BitField end);

// Set all the bits from 0 -> end (exclusive)
// end must be less than sizeof(BitField) * 8
inline BitField SetLowerBitRange(const BitField end);

// Set all the bits from n -> end (inclusive)
// end must more than 0
inline BitField SetUpperBitRange(const BitField end);

// Determine the differing bits between left and right
inline BitField GetMaskDifferences(const BitField left, const BitField right);



// -Implementation-

// Determine if a bit is set
inline bool IsBitSet(const BitField mask, const BitField index) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(index < sizeof(BitField) * 8);
	return (mask >> index) & 1;
}

// Set a bit to on
// the index must be less than sizeof(BitField) * 8
inline BitField SetBit(const BitField mask, const BitField index) {
	MIST_ASSERT(index < sizeof(BitField) * 8);
	return mask | (1 << index);
}

// Toggle a bit from on to off or off to on
inline BitField ToggleBit(BitField mask, const BitField index) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(index < sizeof(BitField) * 8);
	return mask ^ (1 << index);
}

// Set a bit to off
inline BitField UnsetBit(const BitField mask, const BitField index) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(index < sizeof(BitField) * 8);
	return mask & (~(1 << index));
}


// Determine if a flag is set inside of the mask
inline bool IsFlagSet(const BitField mask, const BitField flag) {
	return (mask & flag) == flag;
}

// Determine how many flags are set in the mask
// Concept from: https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan.
// Discovered and shared by: https://github.com/xoorath
inline size_t CountBitsSet(BitField bits) {
	size_t count = 0;
	// While we still have bits left
	while (bits != 0) {
		// Remove the least significant bit
		bits &= bits - 1;
		++count;
	}
	return count;
}

// Get all of the set flags in the mask as their own masks
inline void GetIndividualBitFlags(BitField mask, BitField* bits, size_t* maskCount) {
	MIST_ASSERT(bits != nullptr);
	MIST_ASSERT(maskCount != nullptr);

	(*maskCount) = 0;
	// While we still have bits left
	while (mask != 0) {
		// determine the new mask without the least significant digit
		BitField newMask = mask & (mask - 1);
		// compare the current mask with the new mask using xor to retrieve the changed bit
		bits[(*maskCount)] = mask ^ newMask;

		mask = newMask;
		// Move on to the next mask
		++(*maskCount);
	}
}

// Get all of the indices of the bits set in the mask
inline void GetIndividualBitIndices(const BitField mask, BitField* bitIndices, size_t* indexCount) {
	MIST_ASSERT(bitIndices != nullptr);
	MIST_ASSERT(indexCount != nullptr);

	(*indexCount) = 0;
	// loop through every bit of the mask and determine if they're on or not
	for (BitField i = 0; i < sizeof(BitField); ++i) {
		if (IsBitSet(mask, i)) {
			// Add the index to the array
			bitIndices[(*indexCount)] = i;
			++(*indexCount);
		}
	}
}

// Get a bit mask of all the bit indices
inline BitField GetBitMask(const BitField* bitIndices, const size_t indexCount) {
	MIST_ASSERT(bitIndices != nullptr);

	BitField mask = 0;
	for (size_t i = 0; i < indexCount; ++i) {
		// index must be less than sizeof(BitField) * 8
		MIST_ASSERT(bitIndices[i] < sizeof(BitField) * 8);

		mask |= 1 << (bitIndices[i]);
	}
	return mask;
}

// Get a bit mask for the bit passed in
inline BitField GetBitFlag(const BitField bitIndex) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(bitIndex < sizeof(BitField) * 8);

	return 1 << bitIndex;
}

// Set all the bits from the range begin to end (exclusive)
inline BitField SetBitRange(const BitField begin, const BitField end) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(begin < sizeof(BitField) * 8);
	MIST_ASSERT(end < sizeof(BitField) * 8);
	// If begin is equal or greater than end, the result is 0. This probably isn't the intended range to set.
	MIST_ASSERT(begin < end);

	BitField rangeBitmask = 0;
	// Set the end bit and transform it into a range of those bits
	// end = 4 -> 00001111
	rangeBitmask |= (1 << end) - 1;
	// Remove the bits before the begin index
	// begin = 2 -> 11111100
	// 00001111 & 11111100 = 00001100
	rangeBitmask &= ~((1 << begin) - 1);
	return rangeBitmask;
}

inline BitField GetBitRange(const BitField mask, const BitField begin, const BitField end) {
	// index must be less than sizeof(BitField) * 8
	MIST_ASSERT(begin < sizeof(BitField) * 8);
	MIST_ASSERT(end < sizeof(BitField) * 8);
	// Begin must be less than end or else the mask is 0 and has no effect
	MIST_ASSERT(begin < end);

	BitField rangeBitmask = 0;
	rangeBitmask |= (1 << end) - 1;
	rangeBitmask &= ~((1 << begin) - 1);
	return mask & rangeBitmask;
}

// Set all the bits from 0 -> end (exclusive)
// end must be less than sizeof(BitField) * 8
inline BitField SetLowerBitRange(const BitField end) {
	MIST_ASSERT(end < sizeof(BitField) * 8);
	// end = 4 -> 00000001 -> 00010000 -> 00001111
	return (1 << end) - 1;
}

// Set all the bits from n -> end (inclusive)
// end must more than 0
inline BitField SetUpperBitRange(const BitField end) {
	MIST_ASSERT(end > 0);
	// end = 5 = 8 - 5 = 3 -> 00000001 -> 00001000 -> 00000111 -> 11111000
	return ~((1 << (sizeof(BitField) * 8 - end)) - 1);
}

// Determine the differing bits between left and right
inline BitField GetMaskDifferences(const BitField left, const BitField right) {
	return left ^ right;
}

MIST_NAMESPACE_END
