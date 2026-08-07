#include "ArchitectureComparisonRunner.hpp"

#include "CacheHierarchy.hpp"
#include "CacheStatistics.hpp"
#include "HierarchyStatistics.hpp"
#include "SetAssociativeCache.hpp"

#include <iomanip>
#include <iostream>
#include <utility>

ArchitectureComparisonResult
ArchitectureComparisonRunner::run(
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
) {
    SetAssociativeCache singleCache(
        cacheSize,
        blockSize,
        associativity,
        replacementPolicy,
        writePolicy,
        writeMissPolicy
    );

    for (const MemoryAccess& access : accesses) {
        singleCache.access(access);
    }

    const CacheStatistics& singleStatistics =
        singleCache.getStatistics();

    const double singleAverageCost =
        singleStatistics.calculateAverageAccessCost(
            cacheLatency,
            memoryLatency,
            memoryLatency
        );

    SetAssociativeCache l1(
        l1Size,
        blockSize,
        l1Associativity,
        replacementPolicy,
        writePolicy,
        writeMissPolicy
    );

    SetAssociativeCache l2(
        l2Size,
        blockSize,
        l2Associativity,
        replacementPolicy,
        writePolicy,
        writeMissPolicy
    );

    CacheHierarchy hierarchy(
        std::move(l1),
        std::move(l2)
    );

    for (const MemoryAccess& access : accesses) {
        hierarchy.access(access);
    }

    const HierarchyStatistics& hierarchyStatistics =
        hierarchy.getStatistics();

    const double hierarchyAverageTime =
        hierarchyStatistics.calculateAverageAccessTime(
            l1Latency,
            l2Latency,
            memoryLatency
        );

    const bool hierarchyFaster =
        hierarchyAverageTime < singleAverageCost;

    double improvementPercent = 0.0;

    if (hierarchyFaster && singleAverageCost > 0.0) {
        improvementPercent =
            (
                singleAverageCost -
                hierarchyAverageTime
            ) /
            singleAverageCost *
            100.0;
    } else if (
        !hierarchyFaster &&
        hierarchyAverageTime > 0.0
    ) {
        improvementPercent =
            (
                hierarchyAverageTime -
                singleAverageCost
            ) /
            hierarchyAverageTime *
            100.0;
    }

    return {
        singleAverageCost,
        hierarchyAverageTime,
        singleStatistics.getHitRate(),
        hierarchyStatistics.getGlobalHitRate(),
        hierarchyFaster,
        improvementPercent
    };
}

void ArchitectureComparisonRunner::printResult(
    const ArchitectureComparisonResult& result
) {
    std::cout
        << "\n========================================\n"
        << "Architecture Comparison\n"
        << "========================================\n";

    std::cout << std::fixed
              << std::setprecision(2);

    std::cout
        << "Single-cache hit rate:       "
        << result.singleCacheHitRate
        << "%\n";

    std::cout
        << "Hierarchy global hit rate:   "
        << result.hierarchyGlobalHitRate
        << "%\n\n";

    std::cout
        << "Single-cache average cost:   "
        << result.singleCacheAverageCost
        << " ns\n";

    std::cout
        << "Hierarchy average time:      "
        << result.hierarchyAverageTime
        << " ns\n\n";

    if (result.hierarchyFaster) {
        std::cout
            << "Winner: L1/L2 hierarchy\n"
            << "Improvement: "
            << result.improvementPercent
            << "%\n";
    } else if (
        result.singleCacheAverageCost <
        result.hierarchyAverageTime
    ) {
        std::cout
            << "Winner: Single cache\n"
            << "Improvement: "
            << result.improvementPercent
            << "%\n";
    } else {
        std::cout
            << "Result: Tie\n";
    }

    std::cout
        << "========================================\n";
}