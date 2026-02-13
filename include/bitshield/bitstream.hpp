#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace bitshield::util {

/**
 * Convert text string to bit vector.
 * Each character is converted to 8 bits (MSB first).
 */
std::vector<uint8_t> text_to_bits(const std::string& text);

/**
 * Convert bit vector to text string.
 * Bits are grouped into 8-bit bytes (MSB first).
 * Incomplete bytes at the end are ignored.
 */
std::string bits_to_text(const std::vector<uint8_t>& bits);

/**
 * Convert byte vector to bit vector.
 * Each byte is expanded to 8 bits (MSB first).
 */
std::vector<uint8_t> bytes_to_bits(const std::vector<uint8_t>& bytes);

/**
 * Convert bit vector to byte vector.
 * Bits are grouped into 8-bit bytes (MSB first).
 * Incomplete bytes at the end are padded with zeros.
 */
std::vector<uint8_t> bits_to_bytes(const std::vector<uint8_t>& bits);

} // namespace bitshield::util

