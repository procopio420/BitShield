#include "doctest.h"
#include <bitshield/codecs/repetition.hpp>
#include <vector>
#include <cstdint>
#include <stdexcept>

TEST_CASE("Repetition codec - encode then decode returns original") {
    std::vector<uint8_t> original = {1, 0, 1, 0, 1, 1, 0, 0};
    
    for (int n : {3, 5, 7}) {
        std::vector<uint8_t> encoded = bitshield::codec::repetition::encode(original, n);
        std::vector<uint8_t> decoded = bitshield::codec::repetition::decode(encoded, n);
        
        CHECK(decoded == original);
    }
}

TEST_CASE("Repetition codec - decode corrects single-bit flips") {
    // Encode [1, 0] with n=5: [1,1,1,1,1, 0,0,0,0,0]
    std::vector<uint8_t> original = {1, 0};
    std::vector<uint8_t> encoded = bitshield::codec::repetition::encode(original, 5);
    
    // Flip one bit in first group: [0,1,1,1,1, 0,0,0,0,0]
    std::vector<uint8_t> corrupted = encoded;
    corrupted[0] = 0;
    
    std::vector<uint8_t> decoded = bitshield::codec::repetition::decode(corrupted, 5);
    CHECK(decoded == original);
    
    // Flip one bit in second group: [1,1,1,1,1, 0,0,0,0,1]
    corrupted = encoded;
    corrupted[9] = 1;
    
    decoded = bitshield::codec::repetition::decode(corrupted, 5);
    CHECK(decoded == original);
}

TEST_CASE("Repetition codec - handle incomplete groups") {
    // Encode with n=3, but provide incomplete group
    std::vector<uint8_t> encoded = {1, 1, 1, 0, 0};  // Complete group + incomplete group
    
    std::vector<uint8_t> decoded = bitshield::codec::repetition::decode(encoded, 3);
    CHECK(decoded.size() == 2);
    CHECK(decoded[0] == 1);
    CHECK(decoded[1] == 0);  // Majority of [0,0] is 0
}

TEST_CASE("Repetition codec - invalid n throws") {
    std::vector<uint8_t> bits = {1, 0, 1};
    
    CHECK_THROWS_AS(bitshield::codec::repetition::encode(bits, 0), std::invalid_argument);
    CHECK_THROWS_AS(bitshield::codec::repetition::encode(bits, -1), std::invalid_argument);
    CHECK_THROWS_AS(bitshield::codec::repetition::decode(bits, 0), std::invalid_argument);
    CHECK_THROWS_AS(bitshield::codec::repetition::decode(bits, -1), std::invalid_argument);
}

TEST_CASE("Repetition codec - empty input") {
    std::vector<uint8_t> empty;
    
    std::vector<uint8_t> encoded = bitshield::codec::repetition::encode(empty, 3);
    CHECK(encoded.empty());
    
    std::vector<uint8_t> decoded = bitshield::codec::repetition::decode(empty, 3);
    CHECK(decoded.empty());
}

