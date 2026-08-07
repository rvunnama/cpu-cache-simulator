#include "BenchmarkRunner.hpp"
#include "CacheAccessResult.hpp"
#include "CacheHierarchy.hpp"
#include "MemoryAccess.hpp"
#include "ReplacementPolicy.hpp"
#include "SetAssociativeCache.hpp"
#include "WorkloadGenerator.hpp"
#include "WriteMissPolicy.hpp"
#include "WritePolicy.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

namespace {

bool approximatelyEqual(
    double first,
    double second,
    double tolerance = 0.0001
) {
    return std::fabs(first - second) < tolerance;
}

void testDirectMappedConflictBehavior() {
    SetAssociativeCache cache(
        64,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    const std::vector<std::uint64_t> addresses = {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    };

    for (const std::uint64_t address : addresses) {
        cache.access({
            AccessType::Read,
            address
        });
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 0);
    assert(statistics.getMisses() == 4);
    assert(statistics.getConflictMisses() == 2);

    std::cout
        << "PASS: direct-mapped conflict behavior\n";
}

void testAssociativityReducesConflictMisses() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    const std::vector<std::uint64_t> addresses = {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    };

    for (const std::uint64_t address : addresses) {
        cache.access({
            AccessType::Read,
            address
        });
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 2);
    assert(statistics.getMisses() == 2);

    std::cout
        << "PASS: associativity reduces conflicts\n";
}

void testLruBeatsFifoOnReusePattern() {
    const std::vector<MemoryAccess> accesses = {
        {AccessType::Read, 0x0000},
        {AccessType::Read, 0x0040},
        {AccessType::Read, 0x0000},
        {AccessType::Read, 0x0080},
        {AccessType::Read, 0x0000},
        {AccessType::Read, 0x0040}
    };

    SetAssociativeCache fifo(
        64,
        16,
        2,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    SetAssociativeCache lru(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    for (const MemoryAccess& access : accesses) {
        fifo.access(access);
        lru.access(access);
    }

    assert(
        lru.getStatistics().getHits() >
        fifo.getStatistics().getHits()
    );

    std::cout
        << "PASS: LRU outperforms FIFO on reuse trace\n";
}

void testWriteThroughTraffic() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    cache.access({
        AccessType::Write,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getMemoryReads() == 1);
    assert(statistics.getMemoryWrites() == 2);

    std::cout
        << "PASS: write-through traffic\n";
}

void testWriteBackDirtyEviction() {
    SetAssociativeCache cache(
        16,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    const CacheAccessResult result =
        cache.access({
            AccessType::Read,
            0x0010
        });

    assert(result.evictionOccurred);
    assert(result.dirtyEviction);

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getDirtyEvictions() == 1);
    assert(statistics.getMemoryWrites() == 1);

    std::cout
        << "PASS: write-back dirty eviction\n";
}

void testNoWriteAllocateBypassesCache() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::NoWriteAllocate
    );

    const CacheAccessResult writeResult =
        cache.access({
            AccessType::Write,
            0x0000
        });

    assert(writeResult.bypassed);

    const CacheAccessResult readResult =
        cache.access({
            AccessType::Read,
            0x0000
        });

    assert(!readResult.hit);

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getBypassMisses() == 1);

    std::cout
        << "PASS: no-write-allocate bypass\n";
}

void testMissCategoriesSumCorrectly() {
    SetAssociativeCache cache(
        64,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    const std::vector<std::uint64_t> addresses = {
        0x0000,
        0x0040,
        0x0000,
        0x0010,
        0x0020,
        0x0030,
        0x0040
    };

    for (const std::uint64_t address : addresses) {
        cache.access({
            AccessType::Read,
            address
        });
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    const std::size_t classified =
        statistics.getCompulsoryMisses() +
        statistics.getConflictMisses() +
        statistics.getCapacityMisses() +
        statistics.getBypassMisses();

    assert(classified == statistics.getMisses());

    std::cout
        << "PASS: miss classification totals\n";
}

void testPerformanceMetrics() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    cache.access({
        AccessType::Read,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(
        approximatelyEqual(
            statistics.calculateAmat(
                1.0,
                100.0
            ),
            51.0
        )
    );

    assert(
        approximatelyEqual(
            statistics.calculateAverageAccessCost(
                1.0,
                100.0,
                50.0
            ),
            76.0
        )
    );

    std::cout
        << "PASS: performance metrics\n";
}

void testSyntheticLoopLocality() {
    const std::vector<MemoryAccess> accesses =
        WorkloadGenerator::generate(
            "loop",
            100,
            0,
            16,
            4,
            42
        );

    SetAssociativeCache cache(
        64,
        16,
        4,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    for (const MemoryAccess& access : accesses) {
        cache.access(access);
    }

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getMisses() == 4);
    assert(statistics.getHits() == 96);

    std::cout
        << "PASS: synthetic loop locality\n";
}

void testHierarchyBasicBehavior() {
    SetAssociativeCache l1(
        16,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    SetAssociativeCache l2(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    CacheHierarchy hierarchy(
        std::move(l1),
        std::move(l2)
    );

    const MemoryAccess first {
        AccessType::Read,
        0x0000
    };

    const MemoryAccess second {
        AccessType::Read,
        0x0010
    };

    const HierarchyAccessResult firstResult =
        hierarchy.access(first);

    assert(firstResult.memoryAccess);

    const HierarchyAccessResult hitResult =
        hierarchy.access(first);

    assert(hitResult.l1Hit);

    hierarchy.access(second);

    const HierarchyAccessResult l2Result =
        hierarchy.access(first);

    assert(l2Result.l2Hit);

    std::cout
        << "PASS: hierarchy basic behavior\n";
}

void testBenchmarkSweepProducesRankedResults() {
    const std::vector<MemoryAccess> accesses =
        WorkloadGenerator::generate(
            "loop",
            100,
            0,
            16,
            8,
            42
        );

    const std::vector<BenchmarkResult> results =
        BenchmarkRunner::run(
            accesses,
            1.0,
            100.0,
            50.0
        );

    assert(!results.empty());

    for (
        std::size_t index = 1;
        index < results.size();
        ++index
    ) {
        assert(
            results[index - 1].averageAccessCost <=
            results[index].averageAccessCost
        );
    }

    std::cout
        << "PASS: benchmark sweep ranking\n";
}

}  // namespace

int main() {
    testDirectMappedConflictBehavior();
    testAssociativityReducesConflictMisses();
    testLruBeatsFifoOnReusePattern();
    testWriteThroughTraffic();
    testWriteBackDirtyEviction();
    testNoWriteAllocateBypassesCache();
    testMissCategoriesSumCorrectly();
    testPerformanceMetrics();
    testSyntheticLoopLocality();
    testHierarchyBasicBehavior();
    testBenchmarkSweepProducesRankedResults();

    std::cout
        << "\nAll integration tests passed.\n";

    return 0;
}