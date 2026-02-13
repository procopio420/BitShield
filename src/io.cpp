#include <bitshield/io.hpp>
#include <bitshield/bitstream.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <string>

namespace bitshield::io {

std::pair<int, std::vector<uint8_t>> read_legacy_format(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    int n;
    std::vector<uint8_t> bits;
    
    // Read first token as N
    if (!(file >> n)) {
        throw std::runtime_error("Invalid legacy format: cannot read repetition factor N");
    }
    
    // Read remaining tokens as bits
    int bit;
    while (file >> bit) {
        if (bit != 0 && bit != 1) {
            throw std::runtime_error("Invalid legacy format: bits must be 0 or 1");
        }
        bits.push_back(static_cast<uint8_t>(bit));
    }
    
    return {n, bits};
}

std::vector<uint8_t> read_bit_format(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::vector<uint8_t> bits;
    int bit;
    while (file >> bit) {
        if (bit != 0 && bit != 1) {
            throw std::runtime_error("Invalid bit format: bits must be 0 or 1");
        }
        bits.push_back(static_cast<uint8_t>(bit));
    }
    
    return bits;
}

std::vector<uint8_t> read_text_format(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::string text((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    
    return bitshield::util::text_to_bits(text);
}

void write_bit_format(const std::string& path, const std::vector<uint8_t>& bits) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write file: " + path);
    }
    
    for (size_t i = 0; i < bits.size(); ++i) {
        if (i > 0) {
            file << " ";
        }
        file << static_cast<int>(bits[i]);
    }
}

void write_text_format(const std::string& path, const std::vector<uint8_t>& bits) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write file: " + path);
    }
    
    std::string text = bitshield::util::bits_to_text(bits);
    file << text;
}

} // namespace bitshield::io

