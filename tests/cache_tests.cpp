#include "ReplacementPolicy.hpp"
#include "SetAssociativeCache.hpp"
#include "WritePolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "CacheAccessResult.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void runTrace(
    SetAssociativeCache& cache,
    const std::vector<std::uint64_t>& addresses
) {
    for (const std::uint64_t address : addresses) {
        cache.access({
            AccessType::Read,
            address
        });
    }
}

bool approximatelyEqual(
    double first,
    double second,
    double tolerance = 0.0001
) {
    return std::fabs(first - second) < tolerance;
}

void testDirectMappedConflictMisses() {
    SetAssociativeCache cache(
        64,
        16,
        1,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 0);
    assert(statistics.getMisses() == 4);
    assert(statistics.getTotalAccesses() == 4);

    std::cout
        << "PASS: direct-mapped conflict misses\n";
}

void testTwoWayAssociativityReducesConflicts() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 2);
    assert(statistics.getMisses() == 2);

    std::cout
        << "PASS: two-way associativity reduces conflicts\n";
}

void testFifoReplacement() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0080,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 1);
    assert(statistics.getMisses() == 5);

    std::cout
        << "PASS: FIFO replacement\n";
}

void testLruReplacement() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0080,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getHits() == 2);
    assert(statistics.getMisses() == 4);

    std::cout
        << "PASS: LRU replacement\n";
}

void testStatisticsRates() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(
        approximatelyEqual(
            statistics.getHitRate(),
            50.0
        )
    );

    assert(
        approximatelyEqual(
            statistics.getMissRate(),
            50.0
        )
    );

    std::cout
        << "PASS: statistics rates\n";
}

void testInvalidConfiguration() {
    bool exceptionThrown = false;

    try {
        SetAssociativeCache cache(
            64,
            16,
            3,
            ReplacementPolicy::FIFO,
            WritePolicy::WriteThrough,
            WriteMissPolicy::WriteAllocate
        );
    } catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    std::cout
        << "PASS: invalid configuration rejected\n";
}

void testWriteBackMarksLineDirty() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    const std::vector<CacheSet>& sets =
        cache.getSets();

    assert(sets[0].getLines()[0].dirty);

    std::cout
        << "PASS: write-back marks cache line dirty\n";
}

void testWriteThroughMemoryTraffic() {
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
    assert(statistics.getDirtyEvictions() == 0);

    std::cout
        << "PASS: write-through memory traffic\n";
}

void testWriteBackDirtyEviction() {
    SetAssociativeCache cache(
        32,
        16,
        1,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    cache.access({
        AccessType::Write,
        0x0020
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getMemoryReads() == 2);
    assert(statistics.getMemoryWrites() == 1);
    assert(statistics.getDirtyEvictions() == 1);

    std::cout
        << "PASS: write-back dirty eviction\n";
}

void testWriteAllocateLoadsBlock() {
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

    const CacheAccessResult result =
        cache.access({
            AccessType::Read,
            0x0000
        });

    const bool secondAccessHit = result.hit;

    assert(secondAccessHit);

    std::cout
        << "PASS: write-allocate loads block\n";
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

    cache.access({
        AccessType::Write,
        0x0000
    });

    const CacheAccessResult result =
        cache.access({
            AccessType::Read,
            0x0000
        });

    const bool secondAccessHit = result.hit;

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(!secondAccessHit);
    assert(statistics.getMemoryWrites() == 1);
    assert(statistics.getMemoryReads() == 1);

    std::cout
        << "PASS: no-write-allocate bypasses cache\n";
}

void testAmatCalculation() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000,
        0x0040
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    const double amat =
        statistics.calculateAmat(
            1.0,
            100.0
        );

    assert(
        approximatelyEqual(
            amat,
            51.0
        )
    );

    std::cout << "PASS: AMAT calculation\n";
}

void testAverageAccessCost() {
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

    const double cost =
        statistics.calculateAverageAccessCost(
            1.0,
            100.0,
            50.0
        );

    // One memory read and one memory write
    // across two CPU accesses:
    // 1 + (100 + 50) / 2 = 76
    assert(
        approximatelyEqual(
            cost,
            76.0
        )
    );

    std::cout
        << "PASS: average access cost\n";
}

void testInvalidWritePolicyCombination() {
    bool exceptionThrown = false;

    try {
        SetAssociativeCache cache(
            64,
            16,
            2,
            ReplacementPolicy::LRU,
            WritePolicy::WriteBack,
            WriteMissPolicy::NoWriteAllocate
        );
    } catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    std::cout
        << "PASS: invalid write-policy combination rejected\n";
}

void testEmptyStatistics() {
    CacheStatistics statistics;

    assert(statistics.getTotalAccesses() == 0);
    assert(
        approximatelyEqual(
            statistics.getHitRate(),
            0.0
        )
    );
    assert(
        approximatelyEqual(
            statistics.getMissRate(),
            0.0
        )
    );
    assert(
        approximatelyEqual(
            statistics.calculateAmat(
                1.0,
                100.0
            ),
            1.0
        )
    );
    assert(
        approximatelyEqual(
            statistics.calculateAverageAccessCost(
                1.0,
                100.0,
                100.0
            ),
            0.0
        )
    );

    std::cout
        << "PASS: empty statistics handled safely\n";
}

void testDirtyBitClearedAfterReplacement() {
    SetAssociativeCache cache(
        16,
        16,
        1,
        ReplacementPolicy::FIFO,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    cache.access({
        AccessType::Read,
        0x0010
    });

    const CacheLine& line =
        cache.getSets()
             .at(0)
             .getLines()
             .at(0);

    assert(line.valid);
    assert(!line.dirty);

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getDirtyEvictions() == 1);
    assert(statistics.getMemoryWrites() == 1);

    std::cout
        << "PASS: dirty bit cleared after replacement\n";
}

void testCompulsoryMisses() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0010,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(
        statistics.getCompulsoryMisses() == 2
    );

    assert(
        statistics.getConflictMisses() == 0
    );

    assert(
        statistics.getCapacityMisses() == 0
    );

    std::cout
        << "PASS: compulsory misses classified\n";
}

void testConflictMissClassification() {
    SetAssociativeCache cache(
        64,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0040,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(
        statistics.getCompulsoryMisses() == 2
    );

    assert(
        statistics.getConflictMisses() == 1
    );

    assert(
        statistics.getCapacityMisses() == 0
    );

    std::cout
        << "PASS: conflict miss classified\n";
}

void testCapacityMissClassification() {
    SetAssociativeCache cache(
        32,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    runTrace(cache, {
        0x0000,
        0x0010,
        0x0020,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(
        statistics.getCompulsoryMisses() == 3
    );

    assert(
        statistics.getCapacityMisses() == 1
    );

    std::cout
        << "PASS: capacity miss classified\n";
}

void testBypassMissClassification() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::NoWriteAllocate
    );

    cache.access({
        AccessType::Write,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(statistics.getBypassMisses() == 1);

    std::cout
        << "PASS: bypass miss classified\n";
}

void testTotalClassifiedMisses() {
    SetAssociativeCache cache(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::NoWriteAllocate
    );

    const CacheStatistics& statistics =
        cache.getStatistics();

    const std::size_t classifiedMisses =
    statistics.getCompulsoryMisses() +
    statistics.getConflictMisses() +
    statistics.getCapacityMisses() +
    statistics.getBypassMisses();

    assert(
        classifiedMisses ==
        statistics.getMisses()
    );
}

void testCacheAccessResultReportsEviction() {
    SetAssociativeCache cache(
        16,
        16,
        1,
        ReplacementPolicy::FIFO,
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

    assert(!result.hit);
    assert(result.evictionOccurred);
    assert(result.dirtyEviction);
    assert(result.evictedBlockAddress == 0);

    std::cout
        << "PASS: cache access result reports eviction\n";
}

void testHasBlockDoesNotAffectLru() {
    SetAssociativeCache cache(
        32,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteThrough,
        WriteMissPolicy::WriteAllocate
    );

    cache.access({
        AccessType::Read,
        0x0000
    });

    cache.access({
        AccessType::Read,
        0x0010
    });

    // Inspect block 0 without changing LRU ordering.
    assert(cache.hasBlock(0));

    // This should evict block 0 because it is still
    // the least recently used block.
    cache.access({
        AccessType::Read,
        0x0020
    });

    assert(!cache.hasBlock(0));
    assert(cache.hasBlock(1));
    assert(cache.hasBlock(2));

    std::cout
        << "PASS: cache inspection does not affect LRU\n";
}

}  // namespace

int main() {
    testDirectMappedConflictMisses();
    testTwoWayAssociativityReducesConflicts();
    testFifoReplacement();
    testLruReplacement();
    testStatisticsRates();
    testInvalidConfiguration();
    testWriteBackMarksLineDirty();
    testWriteThroughMemoryTraffic();
    testWriteBackDirtyEviction();
    testWriteAllocateLoadsBlock();
    testNoWriteAllocateBypassesCache();
    testAmatCalculation();
    testAverageAccessCost();
    testInvalidWritePolicyCombination();
    testEmptyStatistics();
    testDirtyBitClearedAfterReplacement();
    testCompulsoryMisses();
    testConflictMissClassification();
    testCapacityMissClassification();
    testBypassMissClassification();
    testTotalClassifiedMisses();
    testCacheAccessResultReportsEviction();
    testHasBlockDoesNotAffectLru();

    std::cout << "\nAll cache tests passed.\n";

    return 0;
}