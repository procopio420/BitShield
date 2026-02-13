#include <bitshield/codecs/hamming74.hpp>
#include <stdexcept>
#include <vector>
#include <cstdint>

namespace bitshield::codec::hamming74 {

std::vector<uint8_t> encode(const std::vector<uint8_t>& data_bits) {
    if (data_bits.size() != 4) {
        throw std::invalid_argument("Hamming(7,4) encode requires exactly 4 data bits");
    }
    
    uint8_t d1 = data_bits[0];
    uint8_t d2 = data_bits[1];
    uint8_t d3 = data_bits[2];
    uint8_t d4 = data_bits[3];
    
    // Calculate parity bits
    uint8_t p1 = d1 ^ d2 ^ d4;  // p1 = d1 ⊕ d2 ⊕ d4
    uint8_t p2 = d1 ^ d3 ^ d4;  // p2 = d1 ⊕ d3 ⊕ d4
    uint8_t p3 = d2 ^ d3 ^ d4;  // p3 = d2 ⊕ d3 ⊕ d4
    
    // Codeword layout: [p1, p2, d1, p3, d2, d3, d4]
    return {p1, p2, d1, p3, d2, d3, d4};
}

std::vector<uint8_t> decode(const std::vector<uint8_t>& codeword) {
    if (codeword.size() != 7) {
        throw std::invalid_argument("Hamming(7,4) decode requires exactly 7 bits");
    }
    
    uint8_t p1 = codeword[0];
    uint8_t p2 = codeword[1];
    uint8_t d1 = codeword[2];
    uint8_t p3 = codeword[3];
    uint8_t d2 = codeword[4];
    uint8_t d3 = codeword[5];
    uint8_t d4 = codeword[6];
    
    // Calculate syndrome
    uint8_t s1 = p1 ^ d1 ^ d2 ^ d4;  // s1 = p1 ⊕ d1 ⊕ d2 ⊕ d4
    uint8_t s2 = p2 ^ d1 ^ d3 ^ d4;  // s2 = p2 ⊕ d1 ⊕ d3 ⊕ d4
    uint8_t s3 = p3 ^ d2 ^ d3 ^ d4;  // s3 = p3 ⊕ d2 ⊕ d3 ⊕ d4
    
    // Syndrome value indicates error position (0 = no error, 1-7 = bit position)
    uint8_t syndrome = (s3 << 2) | (s2 << 1) | s1;
    
    // Correct error if any
    std::vector<uint8_t> corrected = codeword;
    if (syndrome > 0 && syndrome <= 7) {
        // Flip bit at error position (1-indexed, convert to 0-indexed)
        corrected[syndrome - 1] = corrected[syndrome - 1] ^ 1;
    }
    
    // Extract data bits from corrected codeword
    return {corrected[2], corrected[4], corrected[5], corrected[6]};
}

std::vector<uint8_t> encode_bits(const std::vector<uint8_t>& bits) {
    std::vector<uint8_t> encoded;
    encoded.reserve((bits.size() + 3) / 4 * 7);
    
    for (size_t i = 0; i < bits.size(); i += 4) {
        std::vector<uint8_t> data_bits(4, 0);
        for (int j = 0; j < 4 && (i + j) < bits.size(); ++j) {
            data_bits[j] = bits[i + j];
        }
        
        std::vector<uint8_t> codeword = encode(data_bits);
        encoded.insert(encoded.end(), codeword.begin(), codeword.end());
    }
    
    return encoded;
}

std::vector<uint8_t> decode_bits(const std::vector<uint8_t>& encoded) {
    if (encoded.size() % 7 != 0) {
        throw std::invalid_argument("Hamming(7,4) decode requires input size to be a multiple of 7");
    }
    
    std::vector<uint8_t> decoded;
    decoded.reserve(encoded.size() / 7 * 4);
    
    for (size_t i = 0; i < encoded.size(); i += 7) {
        std::vector<uint8_t> codeword(encoded.begin() + i, encoded.begin() + i + 7);
        std::vector<uint8_t> data_bits = decode(codeword);
        decoded.insert(decoded.end(), data_bits.begin(), data_bits.end());
    }
    
    return decoded;
}

} // namespace bitshield::codec::hamming74

