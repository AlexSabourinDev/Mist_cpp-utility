#include "../../include/common/UtilityMacros.h"
#include "../../include/utility/BitManipulations.h"

MIST_NAMESPACE

// -Bit Methods-

// Determine if a flag is set inside of the mask
inline bool IsFlagSet(const size_t mask, const size_t flag) {
	return (mask & flag) == flag;
}

// Determine how many flags are set in the mask
inline size_t CountBitsSet(size_t bits) {
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
inline void GetIndividualBitMasks(size_t mask, size_t** bits, size_t* maskCount) {
	(*maskCount) = 0;
	// While we still have bits left
	while (mask != 0) {
		// determine the new mask without the least significant digit
		size_t newMask = mask & (mask - 1);
		// compare the current mask with the new mask using xor to retrieve the changed bit
		(*bits)[(*maskCount)] = mask ^ newMask;
		// Move on to the next mask
		++(*maskCount);
	}
}

// Get all of the indices of the bits set in the mask
inline void GetIndividualBitIndices(const size_t mask, size_t** bitIndices, size_t* indexCount) {
	(*indexCount) = 0;
	// loop through every bit of the mask and determine if they're on or not
	for (size_t i = 0; i < sizeof(size_t); ++i) {
		if (IsBitSet(mask, i)) {
			// Add the index to the array
			(*bitIndices)[(*indexCount)] = i;
			++(*indexCount);
		}
	}
}

// Get a bit mask of all the bit indices
inline size_t GetBitMask(const size_t* bitIndices, const size_t indexCount) {
	size_t mask = 0;
	for (size_t i = 0; i < indexCount; ++i) {
		mask |= 1 << (bitIndices[i]);
	}
	return mask;
}

// Get a bit mask for the bit passed in
inline size_t GetBitMask(const size_t bitIndex) {
	return 1 << bitIndex;
}

// Set all the bits from the range begin to end (exclusive)
inline size_t SetBitRange(const size_t begin, const size_t end) {
	size_t rangeBitmask = 0;
	// Set the end bit and transform it into a range of those bits
	// end = 4 -> 00001111
	rangeBitmask |= (1 << end) - 1;
	// Remove the bits before the begin index
	// begin = 2 -> 11111100
	// 00001111 & 11111100 = 00001100
	rangeBitmask &= (~(1 << begin)) - 1;
	return rangeBitmask;
}

inline size_t GetBitRange(const size_t mask, const size_t begin, const size_t end) {
	size_t rangeBitmask = 0;
	rangeBitmask |= (1 << end) - 1;
	rangeBitmask &= (~(1 << begin)) - 1;
	return mask & rangeBitmask;
}

// Set all the bits from 0 -> end (exclusive)
inline size_t SetLowerRange(const size_t end) {
	// end = 4 -> 00000001 -> 00010000 -> 00001111
	return (1 << end) - 1;
}

// Set all the bits from n -> end (inclusive)
inline size_t SetUpperRange(const size_t end) {
	// end = 4 -> 00000001 -> 00010000 -> 00001111 -> 11110000
	return (~(1 << end)) - 1;
}

// Determine if a bit is set
inline bool IsBitSet(const size_t mask, const size_t index) {
	return (mask >> index) & 1;
}

// Set a bit to on
inline size_t SetBit(const size_t mask, const size_t index) {
	return mask | (1 << index);
}

// Toggle a bit from on to off or off to on
inline size_t ToggleBit(size_t mask, const size_t index) {
	// Create a mask for the bit value
	size_t bit = 1 << index;
	// & it to determine it's value
	bit = mask & bit;
	// xor that value with a 1 to toggle it
	bit = bit ^ (1 << index);
	
	// unset the bit in the mask and or the new value
	mask |= (~(1 << index));
	mask |= bit;
}

// Set a bit to off
inline size_t UnsetBit(const size_t mask, const size_t index) {
	return mask | (~(1 << index));
}

MIST_NAMESPACE_END