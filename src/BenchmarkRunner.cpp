#include "BenchmarkRunner.hpp"

#include "CacheStatistics.hpp"
#include "SetAssociativeCache.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

std::vector<BenchmarkResult> BenchmarkRunner::run(
    const std::vector<std::uint64_t>& addresses,
    std::size_t cacheSize,
    std::size_t blockSize
) {
    const std::size_t totalLines =
        cacheSize / blockSize;

    const std::vector<std::size_t> associativities = {
        1,
        2,
        4
    };

    const std::vector<ReplacementPolicy> policies = {
        ReplacementPolicy::FIFO,
        ReplacementPolicy::LRU
    };

    std::vector<BenchmarkResult> results;

    for (const std::size_t associativity : associativities) {
        if (
            associativity > totalLines ||
            totalLines % associativity != 0
        ) {
            continue;
        }

        for (const ReplacementPolicy policy : policies) {
            results.push_back(
                runConfiguration(
                    addresses,
                    cacheSize,
                    blockSize,
                    associativity,
                    policy
                )
            );
        }
    }

    return results;
}

BenchmarkResult BenchmarkRunner::runConfiguration(
    const std::vector<std::uint64_t>& addresses,
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    ReplacementPolicy policy
) {
    SetAssociativeCache cache(
        cacheSize,
        blockSize,
        associativity,
        policy
    );

    for (const std::uint64_t address : addresses) {
        cache.access(address);
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    return {
        cacheSize,
        blockSize,
        associativity,
        policy,
        statistics.getHits(),
        statistics.getMisses(),
        statistics.getHitRate()
    };
}

void BenchmarkRunner::printResults(
    const std::vector<BenchmarkResult>& results
) {
    std::cout << "\nBenchmark Results\n";
    std::cout
        << "-------------------------------------------------------------------\n";

    std::cout
        << std::left
        << std::setw(12) << "Cache"
        << std::setw(12) << "Block"
        << std::setw(16) << "Associativity"
        << std::setw(10) << "Policy"
        << std::setw(10) << "Hits"
        << std::setw(10) << "Misses"
        << "Hit Rate\n";

    std::cout
        << "-------------------------------------------------------------------\n";

    for (const BenchmarkResult& result : results) {
        std::cout
            << std::left
            << std::setw(12) << result.cacheSize
            << std::setw(12) << result.blockSize
            << std::setw(16) << result.associativity
            << std::setw(10) << policyToString(result.policy)
            << std::setw(10) << result.hits
            << std::setw(10) << result.misses
            << std::fixed
            << std::setprecision(2)
            << result.hitRate
            << "%\n";
    }
}

const char* BenchmarkRunner::policyToString(
    ReplacementPolicy policy
) {
    switch (policy) {
        case ReplacementPolicy::FIFO:
            return "FIFO";

        case ReplacementPolicy::LRU:
            return "LRU";
    }

    return "Unknown";
}