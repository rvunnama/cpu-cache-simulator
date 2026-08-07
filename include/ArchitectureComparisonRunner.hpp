#ifndef ARCHITECTURE_COMPARISON_RUNNER_HPP
#define ARCHITECTURE_COMPARISON_RUNNER_HPP

#include "MemoryAccess.hpp"
#include "ReplacementPolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "WritePolicy.hpp"

#include <cstddef>
#include <vector>

struct ArchitectureComparisonResult {
    double singleCacheAverageCost;
    double hierarchyAverageTime;

    double singleCacheHitRate;
    double hierarchyGlobalHitRate;

    bool hierarchyFaster;
    double improvementPercent;
};

class ArchitectureComparisonRunner {
public:
    static ArchitectureComparisonResult run(
        const std::vector<MemoryAccess>& accesses,

        std::size_t cacheSize,
        std::size_t blockSize,
        std::size_t associativity,

        std::size_t l1Size,
        std::size_t l1Associativity,

        std::size_t l2Size,
        std::size_t l2Associativity,

        double cacheLatency,
        double l1Latency,
        double l2Latency,
        double memoryLatency,

        ReplacementPolicy replacementPolicy,
        WritePolicy writePolicy,
        WriteMissPolicy writeMissPolicy
    );

    static void printResult(
        const ArchitectureComparisonResult& result
    );
};

#endif