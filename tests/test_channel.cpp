#include "doctest.h"
#include <bitshield/channel.hpp>
#include <vector>
#include <cstdint>
#include <stdexcept>

TEST_CASE("Channel - p=0.0 leaves bits unchanged") {
    std::vector<uint8_t> original = {1, 0, 1, 0, 1, 1, 0, 0};
    
    std::vector<uint8_t> noisy = bitshield::channel::apply_noise(original, 0.0, 42);
    
    CHECK(noisy == original);
}

TEST_CASE("Channel - deterministic output with fixed seed") {
    std::vector<uint8_t> original = {1, 0, 1, 0, 1, 1, 0, 0};
    
    std::vector<uint8_t> noisy1 = bitshield::channel::apply_noise(original, 0.5, 42);
    std::vector<uint8_t> noisy2 = bitshield::channel::apply_noise(original, 0.5, 42);
    
    CHECK(noisy1 == noisy2);
}

TEST_CASE("Channel - p=1.0 flips all bits") {
    std::vector<uint8_t> original = {1, 0, 1, 0, 1, 1, 0, 0};
    
    std::vector<uint8_t> noisy = bitshield::channel::apply_noise(original, 1.0, 42);
    
    CHECK(noisy.size() == original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        CHECK(noisy[i] == (original[i] ^ 1));
    }
}

TEST_CASE("Channel - invalid p throws") {
    std::vector<uint8_t> bits = {1, 0, 1};
    
    CHECK_THROWS_AS(bitshield::channel::apply_noise(bits, -0.1, 42), std::invalid_argument);
    CHECK_THROWS_AS(bitshield::channel::apply_noise(bits, 1.1, 42), std::invalid_argument);
}

TEST_CASE("Channel - empty input") {
    std::vector<uint8_t> empty;
    
    std::vector<uint8_t> noisy = bitshield::channel::apply_noise(empty, 0.5, 42);
    CHECK(noisy.empty());
}

TEST_CASE("Channel - different seeds produce different results") {
    std::vector<uint8_t> original = {1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1};
    
    std::vector<uint8_t> noisy1 = bitshield::channel::apply_noise(original, 0.3, 42);
    std::vector<uint8_t> noisy2 = bitshield::channel::apply_noise(original, 0.3, 123);
    
    // With different seeds, results should be different (very likely)
    bool different = false;
    for (size_t i = 0; i < original.size(); ++i) {
        if (noisy1[i] != noisy2[i]) {
            different = true;
            break;
        }
    }
    CHECK(different);
}

