#include <bitshield/codecs/repetition.hpp>
#include <stdexcept>
#include <vector>
#include <cstdint>

namespace bitshield::codec::repetition {

std::vector<uint8_t> encode(const std::vector<uint8_t>& bits, int n) {
    if (n <= 0) {
        throw std::invalid_argument("Repetition factor n must be > 0");
    }
    
    std::vector<uint8_t> encoded;
    encoded.reserve(bits.size() * n);
    
    for (uint8_t bit : bits) {
        for (int i = 0; i < n; ++i) {
            encoded.push_back(bit);
        }
    }
    
    return encoded;
}

std::vector<uint8_t> decode(const std::vector<uint8_t>& encoded, int n) {
    if (n <= 0) {
        throw std::invalid_argument("Repetition factor n must be > 0");
    }
    
    std::vector<uint8_t> decoded;
    decoded.reserve((encoded.size() + n - 1) / n);
    
    for (size_t i = 0; i < encoded.size(); i += n) {
        int count0 = 0, count1 = 0;
        
        // Count bits in this group
        for (int j = 0; j < n && (i + j) < encoded.size(); ++j) {
            if (encoded[i + j] == 1) {
                count1++;
            } else {
                count0++;
            }
        }
        
        // Majority vote
        decoded.push_back(count1 > count0 ? 1 : 0);
    }
    
    return decoded;
}

} // namespace bitshield::codec::repetition

