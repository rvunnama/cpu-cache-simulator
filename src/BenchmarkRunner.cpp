#include "BenchmarkRunner.hpp"

#include "CacheStatistics.hpp"
#include "SetAssociativeCache.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <string>

std::vector<BenchmarkResult> BenchmarkRunner::run(
    const std::vector<MemoryAccess>& accesses,
    std::size_t cacheSize,
    std::size_t blockSize,
    double cacheAccessTime,
    double memoryPenalty
) {
    const std::size_t totalLines =
        cacheSize / blockSize;

    const std::vector<std::size_t> associativities = {
        1,
        2,
        4
    };

    const std::vector<ReplacementPolicy>
        replacementPolicies = {
            ReplacementPolicy::FIFO,
            ReplacementPolicy::LRU
        };

    const std::vector<WritePolicy> writePolicies = {
        WritePolicy::WriteThrough,
        WritePolicy::WriteBack
    };

    const std::vector<WriteMissPolicy>
        writeMissPolicies = {
            WriteMissPolicy::WriteAllocate,
            WriteMissPolicy::NoWriteAllocate
        };

    std::vector<BenchmarkResult> results;

    for (
        const std::size_t associativity :
        associativities
    ) {
        if (
            associativity > totalLines ||
            totalLines % associativity != 0
        ) {
            continue;
        }

        for (
            const ReplacementPolicy replacementPolicy :
            replacementPolicies
        ) {
            for (
                const WritePolicy writePolicy :
                writePolicies
            ) {
                for (
                    const WriteMissPolicy writeMissPolicy :
                    writeMissPolicies
                ) {
                    results.push_back(
                        runConfiguration(
                            accesses,
                            cacheSize,
                            blockSize,
                            associativity,
                            replacementPolicy,
                            writePolicy,
                            writeMissPolicy,
                            cacheAccessTime,
                            memoryPenalty
                        )
                    );
                }
            }
        }
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const BenchmarkResult& first,
        const BenchmarkResult& second) {
            return first.amat < second.amat;
        }
    );

    return results;
}

BenchmarkResult BenchmarkRunner::runConfiguration(
    const std::vector<MemoryAccess>& accesses,
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    ReplacementPolicy replacementPolicy,
    WritePolicy writePolicy,
    WriteMissPolicy writeMissPolicy,
    double cacheAccessTime,
    double memoryPenalty
) {
    SetAssociativeCache cache(
        cacheSize,
        blockSize,
        associativity,
        replacementPolicy,
        writePolicy,
        writeMissPolicy
    );

    for (const MemoryAccess& access : accesses) {
        cache.access(access);
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    return {
        cacheSize,
        blockSize,
        associativity,
        replacementPolicy,
        writePolicy,
        writeMissPolicy,
        statistics.getHits(),
        statistics.getMisses(),
        statistics.getMemoryReads(),
        statistics.getMemoryWrites(),
        statistics.getDirtyEvictions(),
        statistics.getHitRate(),
        statistics.calculateAmat(
            cacheAccessTime,
            memoryPenalty
        )
    };
}

void BenchmarkRunner::printResults(
    const std::vector<BenchmarkResult>& results
) {
    constexpr int rankWidth = 6;
    constexpr int associativityWidth = 8;
    constexpr int replacementWidth = 8;
    constexpr int writePolicyWidth = 16;
    constexpr int writeMissPolicyWidth = 20;
    constexpr int hitsWidth = 8;
    constexpr int missesWidth = 8;
    constexpr int memoryReadsWidth = 12;
    constexpr int memoryWritesWidth = 12;
    constexpr int dirtyEvictionsWidth = 14;
    constexpr int hitRateWidth = 12;
    constexpr int amatWidth = 12;

    const int tableWidth =
        rankWidth +
        associativityWidth +
        replacementWidth +
        writePolicyWidth +
        writeMissPolicyWidth +
        hitsWidth +
        missesWidth +
        memoryReadsWidth +
        memoryWritesWidth +
        dirtyEvictionsWidth +
        hitRateWidth +
        amatWidth;

    const std::string separator(
        static_cast<std::size_t>(tableWidth),
        '-'
    );

    std::cout << "\nBenchmark Results\n";
    std::cout << separator << '\n';

    std::cout
        << std::left
        << std::setw(rankWidth) << "Rank"
        << std::setw(associativityWidth) << "Assoc."
        << std::setw(replacementWidth) << "Repl."
        << std::setw(writePolicyWidth) << "Write Policy"
        << std::setw(writeMissPolicyWidth) << "Write-Miss Policy"
        << std::setw(hitsWidth) << "Hits"
        << std::setw(missesWidth) << "Misses"
        << std::setw(memoryReadsWidth) << "Mem Reads"
        << std::setw(memoryWritesWidth) << "Mem Writes"
        << std::setw(dirtyEvictionsWidth) << "Dirty Evict."
        << std::setw(hitRateWidth) << "Hit Rate %"
        << std::setw(amatWidth) << "AMAT (ns)"
        << '\n';

    std::cout << separator << '\n';

    for (
        std::size_t index = 0;
        index < results.size();
        ++index
    ) {
        const BenchmarkResult& result = results[index];

        std::cout
            << std::left
            << std::setw(rankWidth)
            << (index + 1)

            << std::setw(associativityWidth)
            << result.associativity

            << std::setw(replacementWidth)
            << replacementPolicyToString(
                   result.replacementPolicy
               )

            << std::setw(writePolicyWidth)
            << writePolicyToString(
                   result.writePolicy
               )

            << std::setw(writeMissPolicyWidth)
            << writeMissPolicyToString(
                   result.writeMissPolicy
               )

            << std::setw(hitsWidth)
            << result.hits

            << std::setw(missesWidth)
            << result.misses

            << std::setw(memoryReadsWidth)
            << result.memoryReads

            << std::setw(memoryWritesWidth)
            << result.memoryWrites

            << std::setw(dirtyEvictionsWidth)
            << result.dirtyEvictions

            << std::fixed
            << std::setprecision(2)

            << std::setw(hitRateWidth)
            << result.hitRate

            << std::setw(amatWidth)
            << result.amat

            << '\n';
    }

    std::cout << separator << '\n';

    if (!results.empty()) {
        const BenchmarkResult& best = results.front();

        std::cout << "\nBest configuration\n";
        std::cout << "------------------\n";

        std::cout << "Associativity: "
                  << best.associativity
                  << "-way\n";

        std::cout << "Replacement policy: "
                  << replacementPolicyToString(
                         best.replacementPolicy
                     )
                  << '\n';

        std::cout << "Write policy: "
                  << writePolicyToString(
                         best.writePolicy
                     )
                  << '\n';

        std::cout << "Write-miss policy: "
                  << writeMissPolicyToString(
                         best.writeMissPolicy
                     )
                  << '\n';

        std::cout << "Hit rate: "
                  << std::fixed
                  << std::setprecision(2)
                  << best.hitRate
                  << "%\n";

        std::cout << "Estimated AMAT: "
                  << best.amat
                  << " ns\n";
    }
}

void BenchmarkRunner::exportCsv(
    const std::vector<BenchmarkResult>& results,
    const std::string& filePath
) {
    std::ofstream outputFile(filePath);

    if (!outputFile.is_open()) {
        throw std::runtime_error(
            "Unable to create CSV file: " + filePath
        );
    }

    outputFile
        << "CacheSize,"
        << "BlockSize,"
        << "Associativity,"
        << "ReplacementPolicy,"
        << "WritePolicy,"
        << "WriteMissPolicy,"
        << "Hits,"
        << "Misses,"
        << "MemoryReads,"
        << "MemoryWrites,"
        << "DirtyEvictions,"
        << "HitRate,"
        << "AMAT\n";

    for (const BenchmarkResult& result : results) {
        outputFile
            << result.cacheSize << ','
            << result.blockSize << ','
            << result.associativity << ','
            << replacementPolicyToString(
                   result.replacementPolicy
               ) << ','
            << writePolicyToString(
                   result.writePolicy
               ) << ','
            << writeMissPolicyToString(
                   result.writeMissPolicy
               ) << ','
            << result.hits << ','
            << result.misses << ','
            << result.memoryReads << ','
            << result.memoryWrites << ','
            << result.dirtyEvictions << ','
            << std::fixed
            << std::setprecision(2)
            << result.hitRate << ','
            << result.amat
            << '\n';
    }
}

const char*
BenchmarkRunner::replacementPolicyToString(
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

const char* BenchmarkRunner::writePolicyToString(
    WritePolicy policy
) {
    switch (policy) {
        case WritePolicy::WriteThrough:
            return "Write-through";

        case WritePolicy::WriteBack:
            return "Write-back";
    }

    return "Unknown";
}

const char*
BenchmarkRunner::writeMissPolicyToString(
    WriteMissPolicy policy
) {
    switch (policy) {
        case WriteMissPolicy::WriteAllocate:
            return "Write-allocate";

        case WriteMissPolicy::NoWriteAllocate:
            return "No-write-allocate";
    }

    return "Unknown";
}