#include "HierarchyBenchmarkRunner.hpp"

#include "CacheHierarchy.hpp"
#include "HierarchyStatistics.hpp"
#include "SetAssociativeCache.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <string>

std::vector<HierarchyBenchmarkResult>
HierarchyBenchmarkRunner::run(
    const std::vector<MemoryAccess>& accesses,
    std::size_t blockSize,
    double l1Latency,
    double l2Latency,
    double memoryLatency,
    ReplacementPolicy replacementPolicy,
    WritePolicy writePolicy,
    WriteMissPolicy writeMissPolicy
) {
    const std::vector<std::size_t> l1Sizes = {
        16,
        32,
        64
    };

    const std::vector<std::size_t> l2Sizes = {
        64,
        128,
        256
    };

    const std::vector<std::size_t> associativities = {
        1,
        2,
        4
    };

    std::vector<HierarchyBenchmarkResult> results;

    for (const std::size_t l1Size : l1Sizes) {
        const std::size_t l1Lines =
            l1Size / blockSize;

        for (const std::size_t l2Size : l2Sizes) {
            const std::size_t l2Lines =
                l2Size / blockSize;

            if (l2Size < l1Size) {
                continue;
            }

            for (
                const std::size_t l1Associativity :
                associativities
            ) {
                if (
                    l1Associativity > l1Lines ||
                    l1Lines % l1Associativity != 0
                ) {
                    continue;
                }

                for (
                    const std::size_t l2Associativity :
                    associativities
                ) {
                    if (
                        l2Associativity > l2Lines ||
                        l2Lines % l2Associativity != 0
                    ) {
                        continue;
                    }

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

                    for (
                        const MemoryAccess& access :
                        accesses
                    ) {
                        hierarchy.access(access);
                    }

                    const HierarchyStatistics& statistics =
                        hierarchy.getStatistics();

                    results.push_back({
                        l1Size,
                        l1Associativity,
                        l2Size,
                        l2Associativity,

                        statistics.getL1Hits(),
                        statistics.getL1Misses(),
                        statistics.getL2Hits(),
                        statistics.getL2Misses(),
                        statistics.getMemoryAccesses(),

                        statistics.getL1HitRate(),
                        statistics.getL2LocalHitRate(),
                        statistics.getGlobalHitRate(),

                        statistics.calculateAverageAccessTime(
                            l1Latency,
                            l2Latency,
                            memoryLatency
                        )
                    });
                }
            }
        }
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const HierarchyBenchmarkResult& first,
           const HierarchyBenchmarkResult& second) {
            return first.averageAccessTime <
                   second.averageAccessTime;
        }
    );

    return results;
}

void HierarchyBenchmarkRunner::printResults(
    const std::vector<HierarchyBenchmarkResult>& results
) {
    constexpr int rankWidth = 6;
    constexpr int sizeWidth = 10;
    constexpr int assocWidth = 10;
    constexpr int countWidth = 10;
    constexpr int rateWidth = 12;
    constexpr int timeWidth = 14;

    const int tableWidth =
        rankWidth +
        sizeWidth +
        assocWidth +
        sizeWidth +
        assocWidth +
        countWidth * 5 +
        rateWidth * 3 +
        timeWidth;

    const std::string separator(
        static_cast<std::size_t>(tableWidth),
        '-'
    );

    std::cout << "\nHierarchy Benchmark Results\n";
    std::cout << separator << '\n';

    std::cout
        << std::left
        << std::setw(rankWidth) << "Rank"
        << std::setw(sizeWidth) << "L1 Size"
        << std::setw(assocWidth) << "L1 Assoc"
        << std::setw(sizeWidth) << "L2 Size"
        << std::setw(assocWidth) << "L2 Assoc"
        << std::setw(countWidth) << "L1 Hits"
        << std::setw(countWidth) << "L1 Miss"
        << std::setw(countWidth) << "L2 Hits"
        << std::setw(countWidth) << "L2 Miss"
        << std::setw(countWidth) << "Memory"
        << std::setw(rateWidth) << "L1 Hit %"
        << std::setw(rateWidth) << "L2 Hit %"
        << std::setw(rateWidth) << "Global %"
        << std::setw(timeWidth) << "Avg Time"
        << '\n';

    std::cout << separator << '\n';

    for (
        std::size_t index = 0;
        index < results.size();
        ++index
    ) {
        const HierarchyBenchmarkResult& result =
            results[index];

        std::cout
            << std::left
            << std::setw(rankWidth) << index + 1
            << std::setw(sizeWidth) << result.l1Size
            << std::setw(assocWidth)
            << result.l1Associativity
            << std::setw(sizeWidth) << result.l2Size
            << std::setw(assocWidth)
            << result.l2Associativity
            << std::setw(countWidth)
            << result.l1Hits
            << std::setw(countWidth)
            << result.l1Misses
            << std::setw(countWidth)
            << result.l2Hits
            << std::setw(countWidth)
            << result.l2Misses
            << std::setw(countWidth)
            << result.memoryAccesses
            << std::fixed
            << std::setprecision(2)
            << std::setw(rateWidth)
            << result.l1HitRate
            << std::setw(rateWidth)
            << result.l2LocalHitRate
            << std::setw(rateWidth)
            << result.globalHitRate
            << std::setw(timeWidth)
            << result.averageAccessTime
            << '\n';
    }

    std::cout << separator << '\n';

    if (!results.empty()) {
        const HierarchyBenchmarkResult& best =
            results.front();

        std::cout << "\nBest hierarchy configuration\n";
        std::cout << "----------------------------\n";
        std::cout << "L1: "
                  << best.l1Size
                  << " bytes, "
                  << best.l1Associativity
                  << "-way\n";

        std::cout << "L2: "
                  << best.l2Size
                  << " bytes, "
                  << best.l2Associativity
                  << "-way\n";

        std::cout << "Average access time: "
                  << std::fixed
                  << std::setprecision(2)
                  << best.averageAccessTime
                  << " ns\n";
    }
}