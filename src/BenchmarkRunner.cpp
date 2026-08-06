#include "BenchmarkRunner.hpp"

#include "CacheStatistics.hpp"
#include "SetAssociativeCache.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<BenchmarkResult> BenchmarkRunner::run(
    const std::vector<MemoryAccess>& accesses,
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
                            writeMissPolicy
                        )
                    );
                }
            }
        }
    }

    return results;
}

BenchmarkResult BenchmarkRunner::runConfiguration(
    const std::vector<MemoryAccess>& accesses,
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    ReplacementPolicy replacementPolicy,
    WritePolicy writePolicy,
    WriteMissPolicy writeMissPolicy
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
        statistics.getHitRate()
    };
}

void BenchmarkRunner::printResults(
    const std::vector<BenchmarkResult>& results
) {
    std::cout << "\nBenchmark Results\n";

    std::cout
        << "------------------------------------------------"
        << "------------------------------------------------"
        << "----------------\n";

    std::cout
        << std::left
        << std::setw(8)  << "Assoc"
        << std::setw(8)  << "Repl."
        << std::setw(16) << "Write"
        << std::setw(20) << "Write Miss"
        << std::setw(8)  << "Hits"
        << std::setw(8)  << "Misses"
        << std::setw(10) << "Mem Read"
        << std::setw(10) << "Mem Write"
        << std::setw(12) << "Dirty Evict"
        << "Hit Rate\n";

    std::cout
        << "------------------------------------------------"
        << "------------------------------------------------"
        << "----------------\n";

    for (const BenchmarkResult& result : results) {
        std::cout
            << std::left
            << std::setw(8)
            << result.associativity
            << std::setw(8)
            << replacementPolicyToString(
                   result.replacementPolicy
               )
            << std::setw(16)
            << writePolicyToString(
                   result.writePolicy
               )
            << std::setw(20)
            << writeMissPolicyToString(
                   result.writeMissPolicy
               )
            << std::setw(8)
            << result.hits
            << std::setw(8)
            << result.misses
            << std::setw(10)
            << result.memoryReads
            << std::setw(10)
            << result.memoryWrites
            << std::setw(12)
            << result.dirtyEvictions
            << std::fixed
            << std::setprecision(2)
            << result.hitRate
            << "%\n";
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
        << "HitRate\n";

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
            << result.hitRate
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