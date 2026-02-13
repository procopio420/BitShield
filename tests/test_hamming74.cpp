#include "doctest.h"
#include <bitshield/codecs/hamming74.hpp>
#include <vector>
#include <cstdint>
#include <stdexcept>

TEST_CASE("Hamming(7,4) - encode/decode round-trip for all 16 4-bit values") {
    for (int i = 0; i < 16; ++i) {
        std::vector<uint8_t> data = {
            static_cast<uint8_t>((i >> 3) & 1),
            static_cast<uint8_t>((i >> 2) & 1),
            static_cast<uint8_t>((i >> 1) & 1),
            static_cast<uint8_t>(i & 1)
        };
        
        std::vector<uint8_t> codeword = bitshield::codec::hamming74::encode(data);
        CHECK(codeword.size() == 7);
        
        std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode(codeword);
        CHECK(decoded == data);
    }
}

TEST_CASE("Hamming(7,4) - decode corrects single-bit errors") {
    std::vector<uint8_t> data = {1, 0, 1, 1};
    std::vector<uint8_t> codeword = bitshield::codec::hamming74::encode(data);
    
    // Test error in each position
    for (int pos = 0; pos < 7; ++pos) {
        std::vector<uint8_t> corrupted = codeword;
        corrupted[pos] = corrupted[pos] ^ 1;  // Flip bit
        
        std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode(corrupted);
        CHECK(decoded == data);
    }
}

TEST_CASE("Hamming(7,4) - handle multiple 4-bit blocks") {
    std::vector<uint8_t> data = {1, 0, 1, 1, 0, 1, 0, 0};  // Two 4-bit blocks
    
    std::vector<uint8_t> encoded = bitshield::codec::hamming74::encode_bits(data);
    CHECK(encoded.size() == 14);  // 2 * 7
    
    std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode_bits(encoded);
    CHECK(decoded == data);
}

TEST_CASE("Hamming(7,4) - pad input that's not multiple of 4") {
    std::vector<uint8_t> data = {1, 0, 1};  // 3 bits
    
    std::vector<uint8_t> encoded = bitshield::codec::hamming74::encode_bits(data);
    CHECK(encoded.size() == 7);  // Padded to 4 bits, then encoded
    
    std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode_bits(encoded);
    CHECK(decoded.size() == 4);
    CHECK(decoded[0] == 1);
    CHECK(decoded[1] == 0);
    CHECK(decoded[2] == 1);
    CHECK(decoded[3] == 0);  // Padded with 0
}

TEST_CASE("Hamming(7,4) - invalid input throws") {
    std::vector<uint8_t> wrong_size = {1, 0, 1};
    
    CHECK_THROWS_AS(bitshield::codec::hamming74::encode(wrong_size), std::invalid_argument);
    
    std::vector<uint8_t> wrong_codeword = {1, 0, 1, 0, 1};
    CHECK_THROWS_AS(bitshield::codec::hamming74::decode(wrong_codeword), std::invalid_argument);
    
    std::vector<uint8_t> not_multiple_of_7 = {1, 0, 1, 0, 1, 0};
    CHECK_THROWS_AS(bitshield::codec::hamming74::decode_bits(not_multiple_of_7), std::invalid_argument);
}

TEST_CASE("Hamming(7,4) - empty input") {
    std::vector<uint8_t> empty;
    
    std::vector<uint8_t> encoded = bitshield::codec::hamming74::encode_bits(empty);
    CHECK(encoded.empty());
    
    std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode_bits(empty);
    CHECK(decoded.empty());
}

