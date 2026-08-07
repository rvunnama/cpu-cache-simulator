#ifndef HIERARCHY_BENCHMARK_RUNNER_HPP
#define HIERARCHY_BENCHMARK_RUNNER_HPP

#include "MemoryAccess.hpp"
#include "ReplacementPolicy.hpp"
#include "WritePolicy.hpp"
#include "WriteMissPolicy.hpp"

#include <cstddef>
#include <vector>

struct HierarchyBenchmarkResult {
    std::size_t l1Size;
    std::size_t l1Associativity;
    std::size_t l2Size;
    std::size_t l2Associativity;

    std::size_t l1Hits;
    std::size_t l1Misses;
    std::size_t l2Hits;
    std::size_t l2Misses;
    std::size_t memoryAccesses;

    double l1HitRate;
    double l2LocalHitRate;
    double globalHitRate;
    double averageAccessTime;
};

class HierarchyBenchmarkRunner {
public:
    static std::vector<HierarchyBenchmarkResult> run(
        const std::vector<MemoryAccess>& accesses,
        std::size_t blockSize,
        double l1Latency,
        double l2Latency,
        double memoryLatency,
        ReplacementPolicy replacementPolicy,
        WritePolicy writePolicy,
        WriteMissPolicy writeMissPolicy
    );

    static void printResults(
        const std::vector<HierarchyBenchmarkResult>& results
    );
};

#endif