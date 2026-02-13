#include <bitshield/metrics.hpp>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <chrono>

namespace bitshield::metrics {

double calculate_ber(const std::vector<uint8_t>& original, const std::vector<uint8_t>& received) {
    if (original.size() != received.size()) {
        throw std::invalid_argument("Bit vectors must have the same size for BER calculation");
    }
    
    if (original.empty()) {
        return 0.0;
    }
    
    size_t errors = 0;
    for (size_t i = 0; i < original.size(); ++i) {
        if (original[i] != received[i]) {
            errors++;
        }
    }
    
    return static_cast<double>(errors) / original.size();
}

double calculate_success_rate(const std::vector<uint8_t>& original, const std::vector<uint8_t>& received) {
    if (original.size() != received.size()) {
        throw std::invalid_argument("Bit vectors must have the same size for success rate calculation");
    }
    
    if (original.empty()) {
        return 1.0;
    }
    
    bool match = true;
    for (size_t i = 0; i < original.size(); ++i) {
        if (original[i] != received[i]) {
            match = false;
            break;
        }
    }
    
    return match ? 1.0 : 0.0;
}

void Timer::start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    running_ = true;
}

void Timer::stop() {
    if (running_) {
        end_time_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
}

double Timer::elapsed_seconds() const {
    if (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - start_time_);
        return duration.count();
    } else {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time_ - start_time_);
        return duration.count();
    }
}

double Timer::elapsed_milliseconds() const {
    return elapsed_seconds() * 1000.0;
}

} // namespace bitshield::metrics

