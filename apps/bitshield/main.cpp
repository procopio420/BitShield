#include <bitshield/codecs/repetition.hpp>
#include <bitshield/codecs/hamming74.hpp>
#include <bitshield/channel.hpp>
#include <bitshield/io.hpp>
#include <bitshield/bitstream.hpp>
#include <bitshield/metrics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <random>
#include <optional>

namespace {

// Simple argument parser
class ArgParser {
public:
    ArgParser(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            args_.push_back(argv[i]);
        }
    }
    
    bool has_flag(const std::string& flag) const {
        return std::find(args_.begin(), args_.end(), flag) != args_.end();
    }
    
    std::string get_value(const std::string& flag, const std::string& default_val = "") const {
        auto it = std::find(args_.begin(), args_.end(), flag);
        if (it != args_.end() && (it + 1) != args_.end()) {
            return *(it + 1);
        }
        return default_val;
    }
    
    std::string get_subcommand() const {
        if (args_.empty()) {
            return "";
        }
        return args_[0];
    }
    
    void print_usage() const {
        std::cout << "BitShield - Error correction lab\n\n";
        std::cout << "Usage: bitshield <command> [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  encode    Encode bits using a codec\n";
        std::cout << "  decode    Decode bits using a codec\n";
        std::cout << "  simulate  Simulate noisy channel transmission\n";
        std::cout << "  benchmark Benchmark codec performance\n\n";
        std::cout << "Examples:\n";
        std::cout << "  bitshield encode --codec repetition --n 5 --text \"hello\" --output encoded.txt\n";
        std::cout << "  bitshield decode --codec repetition --n 5 --input teste.txt --output out.txt\n";
        std::cout << "  bitshield decode --codec hamming --input encoded.txt --output out.txt\n";
        std::cout << "  bitshield simulate --codec repetition --n 5 --text \"hello\" --p 0.02 --trials 1000 --seed 42\n";
        std::cout << "  bitshield benchmark --codec repetition --n 3,5,7 --size 1MB --seed 42\n";
    }
    
private:
    std::vector<std::string> args_;
};

void cmd_encode(const ArgParser& parser) {
    std::string codec = parser.get_value("--codec");
    if (codec.empty()) {
        throw std::runtime_error("--codec is required for encode command");
    }
    
    std::vector<uint8_t> input_bits;
    std::string text = parser.get_value("--text");
    std::string input_file = parser.get_value("--input");
    
    if (!text.empty()) {
        input_bits = bitshield::util::text_to_bits(text);
    } else if (!input_file.empty()) {
        std::string format = parser.get_value("--format", "text");
        if (format == "legacy") {
            auto [n, bits] = bitshield::io::read_legacy_format(input_file);
            input_bits = bits;
        } else {
            input_bits = bitshield::io::read_text_format(input_file);
        }
    } else {
        throw std::runtime_error("Either --text or --input is required");
    }
    
    std::vector<uint8_t> encoded;
    
    if (codec == "repetition") {
        std::string n_str = parser.get_value("--n");
        if (n_str.empty()) {
            throw std::runtime_error("--n is required for repetition codec");
        }
        int n = std::stoi(n_str);
        encoded = bitshield::codec::repetition::encode(input_bits, n);
    } else if (codec == "hamming") {
        encoded = bitshield::codec::hamming74::encode_bits(input_bits);
    } else {
        throw std::runtime_error("Unknown codec: " + codec);
    }
    
    std::string output = parser.get_value("--output");
    if (!output.empty()) {
        std::string format = parser.get_value("--format");
        // For Hamming codec, default to bit format since encoded bits may not be multiple of 8
        if (format.empty()) {
            format = (codec == "hamming") ? "legacy" : "text";
        }
        if (format == "legacy") {
            bitshield::io::write_bit_format(output, encoded);
        } else {
            bitshield::io::write_text_format(output, encoded);
        }
    } else {
        // Output to stdout
        for (uint8_t bit : encoded) {
            std::cout << static_cast<int>(bit);
        }
        std::cout << std::endl;
    }
}

void cmd_decode(const ArgParser& parser) {
    std::string codec = parser.get_value("--codec");
    if (codec.empty()) {
        throw std::runtime_error("--codec is required for decode command");
    }
    
    std::string input_file = parser.get_value("--input");
    if (input_file.empty()) {
        throw std::runtime_error("--input is required for decode command");
    }
    
    std::string format = parser.get_value("--format");
    // For Hamming codec, default to bit format
    if (format.empty()) {
        format = (codec == "hamming") ? "legacy" : "text";
    }
    std::vector<uint8_t> encoded;
    
    if (format == "legacy") {
        if (codec == "hamming") {
            // For Hamming, read pure bit format (no N prefix)
            encoded = bitshield::io::read_bit_format(input_file);
        } else {
            // For repetition, read legacy format with N
            auto [n, bits] = bitshield::io::read_legacy_format(input_file);
            encoded = bits;
            std::string n_str = parser.get_value("--n");
            if (n_str.empty()) {
                // Could use n from file, but for consistency require --n
                throw std::runtime_error("--n is required for repetition codec");
            }
        }
    } else {
        encoded = bitshield::io::read_text_format(input_file);
    }
    
    std::vector<uint8_t> decoded;
    
    if (codec == "repetition") {
        std::string n_str = parser.get_value("--n");
        if (n_str.empty()) {
            throw std::runtime_error("--n is required for repetition codec");
        }
        int n = std::stoi(n_str);
        decoded = bitshield::codec::repetition::decode(encoded, n);
    } else if (codec == "hamming") {
        decoded = bitshield::codec::hamming74::decode_bits(encoded);
    } else {
        throw std::runtime_error("Unknown codec: " + codec);
    }
    
    std::string output = parser.get_value("--output");
    if (!output.empty()) {
        bitshield::io::write_text_format(output, decoded);
    } else {
        // Output to stdout
        std::string text = bitshield::util::bits_to_text(decoded);
        std::cout << text;
    }
}

void cmd_simulate(const ArgParser& parser) {
    std::string codec = parser.get_value("--codec");
    if (codec.empty()) {
        throw std::runtime_error("--codec is required for simulate command");
    }
    
    std::string text = parser.get_value("--text");
    if (text.empty()) {
        throw std::runtime_error("--text is required for simulate command");
    }
    
    std::string p_str = parser.get_value("--p");
    if (p_str.empty()) {
        throw std::runtime_error("--p is required for simulate command");
    }
    double p = std::stod(p_str);
    
    int trials = 1;
    std::string trials_str = parser.get_value("--trials");
    if (!trials_str.empty()) {
        trials = std::stoi(trials_str);
    }
    
    uint32_t seed = 0;
    std::string seed_str = parser.get_value("--seed");
    bool use_seed = !seed_str.empty();
    if (use_seed) {
        seed = std::stoul(seed_str);
    }
    
    std::vector<uint8_t> original_bits = bitshield::util::text_to_bits(text);
    std::vector<uint8_t> encoded;
    
    if (codec == "repetition") {
        std::string n_str = parser.get_value("--n");
        if (n_str.empty()) {
            throw std::runtime_error("--n is required for repetition codec");
        }
        int n = std::stoi(n_str);
        encoded = bitshield::codec::repetition::encode(original_bits, n);
    } else if (codec == "hamming") {
        encoded = bitshield::codec::hamming74::encode_bits(original_bits);
    } else {
        throw std::runtime_error("Unknown codec: " + codec);
    }
    
    size_t total_errors = 0;
    size_t successful_messages = 0;
    bitshield::metrics::Timer timer;
    
    timer.start();
    for (int i = 0; i < trials; ++i) {
        uint32_t trial_seed = use_seed ? seed + i : 0;
        std::optional<uint32_t> opt_seed = trial_seed > 0 ? std::make_optional(trial_seed) : std::nullopt;
        
        std::vector<uint8_t> noisy = bitshield::channel::apply_noise(encoded, p, opt_seed);
        
        std::vector<uint8_t> decoded;
        if (codec == "repetition") {
            int n = std::stoi(parser.get_value("--n"));
            decoded = bitshield::codec::repetition::decode(noisy, n);
        } else {
            decoded = bitshield::codec::hamming74::decode_bits(noisy);
        }
        
        // Calculate errors
        for (size_t j = 0; j < original_bits.size() && j < decoded.size(); ++j) {
            if (original_bits[j] != decoded[j]) {
                total_errors++;
            }
        }
        
        if (decoded == original_bits) {
            successful_messages++;
        }
    }
    timer.stop();
    
    double ber = static_cast<double>(total_errors) / (original_bits.size() * trials);
    double success_rate = static_cast<double>(successful_messages) / trials;
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Simulation Results:\n";
    std::cout << "  Trials: " << trials << "\n";
    std::cout << "  Bit Error Rate (BER): " << ber << "\n";
    std::cout << "  Message Success Rate: " << success_rate << "\n";
    std::cout << "  Time: " << timer.elapsed_milliseconds() << " ms\n";
}

void cmd_benchmark(const ArgParser& parser) {
    std::string codec = parser.get_value("--codec");
    if (codec.empty()) {
        throw std::runtime_error("--codec is required for benchmark command");
    }
    
    std::string n_str = parser.get_value("--n");
    std::string size_str = parser.get_value("--size", "1MB");
    
    // Parse size (simple: assume MB)
    size_t size_bytes = 1024 * 1024;  // Default 1MB
    if (size_str.find("MB") != std::string::npos) {
        size_t mb = std::stoul(size_str);
        size_bytes = mb * 1024 * 1024;
    }
    
    uint32_t seed = 0;
    std::string seed_str = parser.get_value("--seed");
    if (!seed_str.empty()) {
        seed = std::stoul(seed_str);
    }
    
    // Generate test data
    std::vector<uint8_t> test_bits;
    test_bits.reserve(size_bytes * 8);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<unsigned int> dist(0, 1);
    for (size_t i = 0; i < size_bytes * 8; ++i) {
        test_bits.push_back(static_cast<uint8_t>(dist(rng)));
    }
    
    bitshield::metrics::Timer timer;
    
    if (codec == "repetition") {
        if (n_str.empty()) {
            throw std::runtime_error("--n is required for repetition codec");
        }
        
        // Parse comma-separated n values
        std::vector<int> n_values;
        std::istringstream iss(n_str);
        std::string token;
        while (std::getline(iss, token, ',')) {
            n_values.push_back(std::stoi(token));
        }
        
        for (int n : n_values) {
            timer.start();
            std::vector<uint8_t> encoded = bitshield::codec::repetition::encode(test_bits, n);
            std::vector<uint8_t> decoded = bitshield::codec::repetition::decode(encoded, n);
            timer.stop();
            
            double throughput = (test_bits.size() * 2) / timer.elapsed_seconds() / 1e6;  // Mbps
            std::cout << "Repetition(n=" << n << "): " << timer.elapsed_milliseconds() 
                      << " ms, Throughput: " << std::fixed << std::setprecision(2) 
                      << throughput << " Mbps\n";
        }
    } else if (codec == "hamming") {
        timer.start();
        std::vector<uint8_t> encoded = bitshield::codec::hamming74::encode_bits(test_bits);
        std::vector<uint8_t> decoded = bitshield::codec::hamming74::decode_bits(encoded);
        timer.stop();
        
        double throughput = (test_bits.size() * 2) / timer.elapsed_seconds() / 1e6;  // Mbps
        std::cout << "Hamming(7,4): " << timer.elapsed_milliseconds() 
                  << " ms, Throughput: " << std::fixed << std::setprecision(2) 
                  << throughput << " Mbps\n";
    } else {
        throw std::runtime_error("Unknown codec: " + codec);
    }
}

} // anonymous namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        ArgParser parser(argc, argv);
        parser.print_usage();
        return 1;
    }
    
    try {
        ArgParser parser(argc, argv);
        std::string cmd = parser.get_subcommand();
        
        if (cmd == "encode") {
            cmd_encode(parser);
        } else if (cmd == "decode") {
            cmd_decode(parser);
        } else if (cmd == "simulate") {
            cmd_simulate(parser);
        } else if (cmd == "benchmark") {
            cmd_benchmark(parser);
        } else {
            std::cerr << "Unknown command: " << cmd << "\n";
            parser.print_usage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

