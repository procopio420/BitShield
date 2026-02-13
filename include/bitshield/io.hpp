#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <utility>

namespace bitshield::io {

/**
 * Read legacy format file.
 * Format: First token is N (repetition factor), rest are 0/1 bits.
 * 
 * @param path File path
 * @return Pair of (N, bit vector)
 * @throws std::runtime_error if file cannot be read
 */
std::pair<int, std::vector<uint8_t>> read_legacy_format(const std::string& path);

/**
 * Read bit format file (space-separated 0/1 values, no N prefix).
 * 
 * @param path File path
 * @return Bit vector
 * @throws std::runtime_error if file cannot be read
 */
std::vector<uint8_t> read_bit_format(const std::string& path);

/**
 * Read text format file.
 * Raw text is converted to bytes, then to bits.
 * 
 * @param path File path
 * @return Bit vector
 * @throws std::runtime_error if file cannot be read
 */
std::vector<uint8_t> read_text_format(const std::string& path);

/**
 * Write bits in legacy format (space-separated 0/1 values).
 * 
 * @param path File path
 * @param bits Bit vector to write
 * @throws std::runtime_error if file cannot be written
 */
void write_bit_format(const std::string& path, const std::vector<uint8_t>& bits);

/**
 * Write bits as text (decoded from bits).
 * 
 * @param path File path
 * @param bits Bit vector to write
 * @throws std::runtime_error if file cannot be written
 */
void write_text_format(const std::string& path, const std::vector<uint8_t>& bits);

} // namespace bitshield::io

