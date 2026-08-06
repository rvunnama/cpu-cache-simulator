#ifndef BENCHMARK_RUNNER_HPP
#define BENCHMARK_RUNNER_HPP

#include "ReplacementPolicy.hpp"
#include "MemoryAccess.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

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
        const std::vector<MemoryAccess>& accesses,
        std::size_t cacheSize,
        std::size_t blockSize
    );

    static void printResults(
        const std::vector<BenchmarkResult>& results
    );

    static void exportCsv(
        const std::vector<BenchmarkResult>& results,
        const std::string& filePath
    );

private:
    static BenchmarkResult runConfiguration(
        const std::vector<MemoryAccess>& accesses,
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