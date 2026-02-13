#pragma once

#include <vector>
#include <cstdint>

namespace bitshield::codec::hamming74 {

/**
 * Encode 4 data bits into a 7-bit Hamming codeword.
 * Codeword layout: [p1, p2, d1, p3, d2, d3, d4]
 * where p1, p2, p3 are parity bits and d1-d4 are data bits.
 * 
 * @param data_bits 4-bit data vector
 * @return 7-bit codeword
 * @throws std::invalid_argument if data_bits.size() != 4
 */
std::vector<uint8_t> encode(const std::vector<uint8_t>& data_bits);

/**
 * Decode a 7-bit Hamming codeword to 4 data bits with error correction.
 * Corrects up to 1 bit error per codeword.
 * 
 * @param codeword 7-bit codeword
 * @return 4-bit data vector
 * @throws std::invalid_argument if codeword.size() != 7
 */
std::vector<uint8_t> decode(const std::vector<uint8_t>& codeword);

/**
 * Encode a bit vector using Hamming(7,4).
 * Input is padded with zeros if not a multiple of 4 bits.
 * 
 * @param bits Input bit vector
 * @return Encoded bit vector (multiple of 7 bits)
 */
std::vector<uint8_t> encode_bits(const std::vector<uint8_t>& bits);

/**
 * Decode a bit vector using Hamming(7,4).
 * Input must be a multiple of 7 bits.
 * 
 * @param encoded Encoded bit vector
 * @return Decoded bit vector (multiple of 4 bits)
 * @throws std::invalid_argument if encoded.size() is not a multiple of 7
 */
std::vector<uint8_t> decode_bits(const std::vector<uint8_t>& encoded);

} // namespace bitshield::codec::hamming74

