#pragma once

#include "../common/UtilityMacros.h"

// This file provides a series of bit manipulation methods in order to clean up bit code

MIST_NAMESPACE

// -Bit Methods-

// Determine if a flag is set inside of the mask
inline bool IsFlagSet(const size_t mask, const size_t flag);

// Determine how many flags are set in the mask
inline size_t CountBitsSet(size_t bits);

// Get all of the set flags in the mask as their own masks
// @Warning: the bits array must have enough elements to fit sizeof(size_t)
inline void GetIndividualBitMasks(size_t mask, size_t** bits, size_t* maskCount);

// Get all of the indices of the bits set in the mask
// @Warning: the bits array must have enough elements to fit sizeof(size_t)
inline void GetIndividualBitIndices(const size_t mask, size_t** bitIndices, size_t* indexCount);

// Get a bit mask of all the bit indices
inline size_t GetBitMask(const size_t* bitIndices, const size_t indexCount);

// Get a bit mask for the bit passed in
inline size_t GetBitMask(const size_t bitIndex);

// Set all the bits from the range begin to end
inline size_t SetBitRange(const size_t begin, const size_t end);

// Retrieve the mask in the range
inline size_t GetBitRange(const size_t mask, const size_t begin, const size_t end);

// Set all the bits from 0 -> end (exclusive)
inline size_t SetLowerRange(const size_t end);

// Set all the bits from n -> end (inclusive)
inline size_t SetUpperRange(const size_t end);

// Determine if a bit is set
inline bool IsBitSet(const size_t mask, const size_t index);

// Set a bit to on
inline size_t SetBit(const size_t mask, const size_t index);

// Toggle a bit from on to off or off to on
inline size_t ToggleBit(size_t mask, const size_t index);

// Set a bit to off
inline size_t UnsetBit(const size_t mask, const size_t index);


MIST_NAMESPACE_END