#ifndef BENCHMARK_RUNNER_HPP
#define BENCHMARK_RUNNER_HPP

#include "MemoryAccess.hpp"
#include "ReplacementPolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "WritePolicy.hpp"

#include <cstddef>
#include <string>
#include <vector>

struct BenchmarkResult {
    std::size_t cacheSize;
    std::size_t blockSize;
    std::size_t associativity;

    ReplacementPolicy replacementPolicy;
    WritePolicy writePolicy;
    WriteMissPolicy writeMissPolicy;

    std::size_t hits;
    std::size_t misses;
    std::size_t memoryReads;
    std::size_t memoryWrites;
    std::size_t dirtyEvictions;
    std::size_t compulsoryMisses;
    std::size_t conflictMisses;
    std::size_t capacityMisses;
    std::size_t bypassMisses;

    double hitRate;
    double amat;
    double averageAccessCost;
};

class BenchmarkRunner {
public:
    static std::vector<BenchmarkResult> run(
        const std::vector<MemoryAccess>& accesses,
        double cacheAccessTime,
        double memoryReadPenalty,
        double memoryWritePenalty
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
        ReplacementPolicy replacementPolicy,
        WritePolicy writePolicy,
        WriteMissPolicy writeMissPolicy,
        double cacheAccessTime,
        double memoryReadPenalty,
        double memoryWritePenalty
    );

    static const char* replacementPolicyToString(
        ReplacementPolicy policy
    );

    static const char* writePolicyToString(
        WritePolicy policy
    );

    static const char* writeMissPolicyToString(
        WriteMissPolicy policy
    );
};

#endif