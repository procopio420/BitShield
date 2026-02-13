#pragma once

#include <vector>
#include <cstdint>
#include <optional>

namespace bitshield::channel {

/**
 * Apply bit-flip noise to a bit vector with probability p.
 * 
 * @param bits Input bit vector
 * @param p Bit-flip probability (0.0 to 1.0)
 * @param seed Optional random seed for determinism
 * @return Bit vector with noise applied
 * @throws std::invalid_argument if p < 0.0 or p > 1.0
 */
std::vector<uint8_t> apply_noise(
    const std::vector<uint8_t>& bits,
    double p,
    std::optional<uint32_t> seed = std::nullopt
);

} // namespace bitshield::channel

