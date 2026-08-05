#ifndef BENCHMARK_RUNNER_HPP
#define BENCHMARK_RUNNER_HPP

#include "ReplacementPolicy.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

struct BenchmarkResult {
    std::size_t cacheSize;
    std::size_t blockSize;
    std::size_t associativity;
    ReplacementPolicy policy;
    std::size_t hits;
    std::size_t misses;
    double hitRate;
};

class BenchmarkRunner {
public:
    static std::vector<BenchmarkResult> run(
        const std::vector<std::uint64_t>& addresses,
        std::size_t cacheSize,
        std::size_t blockSize
    );

    static void printResults(
        const std::vector<BenchmarkResult>& results
    );

private:
    static BenchmarkResult runConfiguration(
        const std::vector<std::uint64_t>& addresses,
        std::size_t cacheSize,
        std::size_t blockSize,
        std::size_t associativity,
        ReplacementPolicy policy
    );

    static const char* policyToString(
        ReplacementPolicy policy
    );
};

#endif