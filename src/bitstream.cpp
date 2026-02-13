#include <bitshield/bitstream.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace bitshield::util {

std::vector<uint8_t> text_to_bits(const std::string& text) {
    std::vector<uint8_t> bits;
    bits.reserve(text.size() * 8);
    
    for (char c : text) {
        uint8_t byte = static_cast<uint8_t>(c);
        for (int i = 7; i >= 0; --i) {
            bits.push_back((byte >> i) & 1);
        }
    }
    
    return bits;
}

std::string bits_to_text(const std::vector<uint8_t>& bits) {
    std::string text;
    text.reserve(bits.size() / 8);
    
    for (size_t i = 0; i + 7 < bits.size(); i += 8) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; ++j) {
            byte |= (bits[i + j] << (7 - j));
        }
        text += static_cast<char>(byte);
    }
    
    return text;
}

std::vector<uint8_t> bytes_to_bits(const std::vector<uint8_t>& bytes) {
    std::vector<uint8_t> bits;
    bits.reserve(bytes.size() * 8);
    
    for (uint8_t byte : bytes) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((byte >> i) & 1);
        }
    }
    
    return bits;
}

std::vector<uint8_t> bits_to_bytes(const std::vector<uint8_t>& bits) {
    std::vector<uint8_t> bytes;
    bytes.reserve((bits.size() + 7) / 8);
    
    for (size_t i = 0; i < bits.size(); i += 8) {
        uint8_t byte = 0;
        for (int j = 0; j < 8 && (i + j) < bits.size(); ++j) {
            byte |= (bits[i + j] << (7 - j));
        }
        bytes.push_back(byte);
    }
    
    return bytes;
}

} // namespace bitshield::util

