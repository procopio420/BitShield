#pragma once

#include <vector>
#include <cstdint>
#include <chrono>

namespace bitshield::metrics {

/**
 * Calculate Bit Error Rate (BER) between two bit vectors.
 * 
 * @param original Original bit vector
 * @param received Received bit vector
 * @return BER (errors / total_bits)
 * @throws std::invalid_argument if vectors have different sizes
 */
double calculate_ber(const std::vector<uint8_t>& original, const std::vector<uint8_t>& received);

/**
 * Calculate message success rate.
 * A message is considered successful if all bits match.
 * 
 * @param original Original bit vector
 * @param received Received bit vector
 * @return Success rate (successful_messages / total_messages)
 * @throws std::invalid_argument if vectors have different sizes
 */
double calculate_success_rate(const std::vector<uint8_t>& original, const std::vector<uint8_t>& received);

/**
 * Simple timer for benchmarking.
 */
class Timer {
public:
    void start();
    void stop();
    double elapsed_seconds() const;
    double elapsed_milliseconds() const;

private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool running_ = false;
};

} // namespace bitshield::metrics

