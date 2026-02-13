#pragma once

#include <vector>
#include <cstdint>

namespace bitshield::codec::repetition {

/**
 * Encode bits using repetition code.
 * Each bit is repeated n times.
 * 
 * @param bits Input bit vector
 * @param n Repetition factor (must be > 0)
 * @return Encoded bit vector
 * @throws std::invalid_argument if n <= 0
 */
std::vector<uint8_t> encode(const std::vector<uint8_t>& bits, int n);

/**
 * Decode bits using repetition code with majority vote.
 * Groups of n bits are decoded by majority vote.
 * 
 * @param encoded Encoded bit vector
 * @param n Repetition factor (must be > 0)
 * @return Decoded bit vector
 * @throws std::invalid_argument if n <= 0
 */
std::vector<uint8_t> decode(const std::vector<uint8_t>& encoded, int n);

} // namespace bitshield::codec::repetition

