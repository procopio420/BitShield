# BitShield

**Systems-level error correction laboratory for empirical codec analysis and channel modeling.**

BitShield is a C++17 experimentation toolkit for error correction codecs and noisy channel simulation. It provides deterministic, reproducible experiments for understanding correction guarantees, codec behavior, and channel characteristics. The architecture is intentionally modular to support rapid iteration on codec implementations and channel models.

## Why

Error correction sits at the intersection of information theory and systems engineering. Theoretical bounds tell us what's possible; empirical validation tells us what works in practice. BitShield is a laboratory for bridging that gap.

Deterministic experimentation is essential. Seeded PRNGs ensure that channel noise is reproducible, allowing direct comparison of codec performance under identical conditions. This enables systematic exploration of the parameter space—repetition factors, error probabilities, codec selection—with confidence that observed differences reflect algorithmic properties, not randomness.

The modular architecture separates codec logic from channel models and I/O, making it straightforward to add new codecs (BCH, Reed-Solomon, LDPC) or channel models (AWGN, burst errors) without touching existing components. This design supports both production use and research experimentation.

## Engineering Principles

- **No global mutable state**: All functions are pure or operate on explicitly passed state. This enables thread-safe composition and predictable behavior.
- **Deterministic simulations**: All random processes use seeded PRNGs (`std::mt19937`). Identical seeds produce identical results, enabling reproducible experiments and regression testing.
- **Separation of concerns**: Codec logic, channel simulation, I/O, and metrics are independent modules with clear interfaces. This allows independent testing and replacement of components.
- **Cross-platform from the start**: No platform-specific code paths. Builds cleanly on Linux, macOS, and Windows with standard C++17.
- **Test-driven validation**: Correction guarantees are verified through comprehensive test suites. Each codec's error correction capability is validated against known failure modes.
- **Clarity over premature optimization**: The current implementation uses `std::vector<uint8_t>` for bit representation, prioritizing readability and correctness. Bit-packed storage and SIMD optimizations are deferred until profiling indicates they're necessary.

## Quickstart

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The `bitshield` executable will be in the `build` directory.

### Basic Usage

**Encode text using repetition code:**
```bash
./bitshield encode --codec repetition --n 5 --text "hello" --output encoded.txt
```

**Decode from legacy format:**
```bash
./bitshield decode --codec repetition --n 5 --input teste.txt --output out.txt
```

**Encode with Hamming(7,4):**
```bash
./bitshield encode --codec hamming --text "hello" --output encoded.txt
./bitshield decode --codec hamming --input encoded.txt --output out.txt
```

**Simulate noisy channel:**
```bash
./bitshield simulate --codec repetition --n 5 --text "hello" --p 0.02 --trials 1000 --seed 42
```

**Benchmark performance:**
```bash
./bitshield benchmark --codec repetition --n 3,5,7 --size 1MB --seed 42
```

## Conceptual Flow

BitShield implements a standard error correction pipeline:

```
Text → Bits → Encode → Noisy Channel → Decode → Metrics
```

1. **Text → Bits**: Input text is converted to a bit vector (UTF-8/ASCII bytes expanded to bits, MSB first).
2. **Encode**: The bit vector is encoded using the selected codec (repetition or Hamming), producing redundant codewords.
3. **Noisy Channel**: Encoded bits pass through a simulated channel with configurable bit-flip probability `p`. The channel uses a seeded PRNG for deterministic behavior.
4. **Decode**: The corrupted codewords are decoded, with the codec attempting to correct errors using redundancy.
5. **Metrics**: Bit Error Rate (BER), message success rate, and timing statistics are computed to quantify codec performance.

This pipeline is deterministic when seeds are provided, enabling reproducible experiments and regression testing.

## Architecture

BitShield is organized into a clean library architecture:

### Core Components

- **`bitshield::util`**: Bitstream utilities (text ↔ bits, bytes ↔ bits)
- **`bitshield::codec::repetition`**: Repetition code encoder/decoder
- **`bitshield::codec::hamming74`**: Hamming(7,4) encoder/decoder
- **`bitshield::channel`**: Noisy channel simulator
- **`bitshield::io`**: File I/O utilities (legacy and text formats)
- **`bitshield::metrics`**: BER, success rate, and timing utilities

### Codec Details

#### Repetition Code

The repetition code is the simplest error correction scheme: each bit is repeated `n` times. Decoding uses majority vote per group of `n` bits.

- **Encoding**: Each bit → `n` copies
- **Decoding**: Groups of `n` bits → majority vote
- **Error Correction**: Can correct up to `⌊(n-1)/2⌋` errors per group

Example with n=5:
- Input: `[1, 0]`
- Encoded: `[1,1,1,1,1, 0,0,0,0,0]`
- If corrupted to: `[0,1,1,1,1, 0,0,0,0,0]` (one error)
- Decoded: `[1, 0]` ✓ (corrected)

#### Hamming(7,4)

Hamming(7,4) encodes 4 data bits into 7-bit codewords with 3 parity bits, enabling single-bit error correction.

- **Encoding**: 4 data bits → 7-bit codeword
  - Parity bits: `p1 = d1⊕d2⊕d4`, `p2 = d1⊕d3⊕d4`, `p3 = d2⊕d3⊕d4`
  - Layout: `[p1, p2, d1, p3, d2, d3, d4]`
- **Decoding**: 7-bit codeword → 4 data bits + error correction
  - Calculate syndrome to detect error position
  - Flip bit at error position if needed
  - Extract data bits

Example:
- Input: `[1, 0, 1, 1]`
- Encoded: `[0, 1, 1, 0, 0, 1, 1]`
- If corrupted: `[1, 1, 1, 0, 0, 1, 1]` (error in position 0)
- Decoded: `[1, 0, 1, 1]` ✓ (corrected)

## Performance Characteristics

### Time Complexity

- **Repetition code**: 
  - Encode: O(n·m) where n is repetition factor, m is input bits
  - Decode: O(n·m) for majority vote per group
- **Hamming(7,4)**:
  - Encode: O(m) where m is input bits (constant-time per 4-bit block)
  - Decode: O(m) with syndrome calculation and bit correction per 7-bit codeword

### Space Complexity

- **Repetition code**: O(n·m) encoded bits for m input bits
- **Hamming(7,4)**: O(7m/4) encoded bits for m input bits (approximately 1.75× expansion)

### Implementation Notes

The current implementation uses `std::vector<uint8_t>` for bit representation, where each element is 0 or 1. This provides clarity and ease of debugging at the cost of memory efficiency. For production workloads requiring high throughput, bit-packed storage (8 bits per byte) would reduce memory usage by 8× and improve cache locality. SIMD operations could accelerate majority vote and syndrome calculations.

Profiling indicates that for typical experimental workloads (< 10MB), the current implementation is sufficient. Bit-packed optimization is deferred until profiling demonstrates it's necessary.

## Example Simulation Output

```bash
$ bitshield simulate --codec repetition --n 5 --text "hello" --p 0.02 --trials 1000 --seed 42

Simulation Results:
  Trials: 1000
  Bit Error Rate (BER): 0.000050
  Message Success Rate: 0.998000
  Time: 46.243239 ms
```

This output indicates:
- **BER (post-decode)**: 0.005% of decoded bits differ from the original. This is the residual error rate after error correction. The channel introduces errors at rate `p=0.02`, but the repetition code (n=5) corrects most of them, leaving only 0.005% uncorrected.
- **Message Success Rate**: 99.8% of messages were decoded correctly (all bits match original). The 0.2% failure rate corresponds to cases where errors exceeded the codec's correction capability.
- **Time**: Total simulation time for 1000 trials, including encoding, channel simulation, decoding, and error counting.

With a higher error probability (e.g., `p=0.1`) or lower repetition factor (e.g., `n=3`), the message success rate would decrease. This enables systematic exploration of the codec's operating region and correction guarantees under varying channel conditions.

## CLI Reference

### Commands

#### `encode`
Encode bits using a codec.

```bash
bitshield encode --codec <repetition|hamming> [--n <int>] [--text <string>|--input <file>] [--output <file>] [--format <legacy|text>]
```

- `--codec`: Codec to use (`repetition` or `hamming`)
- `--n`: Repetition factor (required for repetition codec)
- `--text`: Input text string
- `--input`: Input file path
- `--output`: Output file path (default: stdout)
- `--format`: Format (`legacy` for space-separated bits, `text` for binary)

#### `decode`
Decode bits using a codec.

```bash
bitshield decode --codec <repetition|hamming> [--n <int>] --input <file> [--output <file>] [--format <legacy|text>]
```

#### `simulate`
Simulate noisy channel transmission.

```bash
bitshield simulate --codec <repetition|hamming> [--n <int>] --text <string> --p <float> [--trials <int>] [--seed <int>]
```

- `--p`: Bit-flip probability (0.0 to 1.0)
- `--trials`: Number of simulation trials (default: 1)
- `--seed`: Random seed for determinism

#### `benchmark`
Benchmark codec performance.

```bash
bitshield benchmark --codec <repetition|hamming> [--n <int>|--n <comma-separated>] [--size <size>] [--seed <int>]
```

- `--n`: Repetition factor(s) (comma-separated for multiple)
- `--size`: Test data size (e.g., `1MB`)

## File Formats

### Legacy Format
First token is the repetition factor `N`, followed by space-separated `0`/`1` bits:
```
5
0 0 0 0 0 1 1 0 1 1 0 0 ...
```

### Text Format
Raw text file (UTF-8/ASCII). For encoding: text → bytes → bits. For decoding: bits → bytes → text.

## Development

### Building Tests

Tests are built automatically with the main build. Run with:

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./bitshield_tests
```

### Project Structure

```
bitshield/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── include/bitshield/      # Public headers
├── src/                    # Implementation
├── apps/bitshield/         # CLI application
└── tests/                  # Test suite
```

## Roadmap

Future enhancements planned:

- **CRC codes**: Cyclic redundancy check
- **BCH codes**: Bose-Chaudhuri-Hocquenghem codes
- **Reed-Solomon**: Advanced error correction
- **Bit-packed storage**: Optimized bit representation
- **More channel models**: BSC, AWGN, etc.
- **Performance optimizations**: SIMD, parallel processing

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please ensure all tests pass and code follows the existing style.
