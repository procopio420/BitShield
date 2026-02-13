#include <bitshield/channel.hpp>
#include <stdexcept>
#include <random>
#include <vector>
#include <cstdint>

namespace bitshield::channel {

std::vector<uint8_t> apply_noise(
    const std::vector<uint8_t>& bits,
    double p,
    std::optional<uint32_t> seed
) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Noise probability p must be between 0.0 and 1.0");
    }
    
    std::vector<uint8_t> noisy_bits = bits;
    
    // Initialize random number generator
    std::mt19937 rng;
    if (seed.has_value()) {
        rng.seed(seed.value());
    } else {
        std::random_device rd;
        rng.seed(rd());
    }
    
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    // Apply bit-flip noise
    for (size_t i = 0; i < noisy_bits.size(); ++i) {
        if (dist(rng) < p) {
            noisy_bits[i] = noisy_bits[i] ^ 1;  // Flip bit
        }
    }
    
    return noisy_bits;
}

} // namespace bitshield::channel

