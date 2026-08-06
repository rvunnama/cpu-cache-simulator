#include "ReplacementPolicy.hpp"
#include "SetAssociativeCache.hpp"
#include "WritePolicy.hpp"
#include "WriteMissPolicy.hpp"

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

    const bool secondAccessHit = cache.access({
        AccessType::Read,
        0x0000
    });

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

    const bool secondAccessHit = cache.access({
        AccessType::Read,
        0x0000
    });

    const CacheStatistics& statistics =
        cache.getStatistics();

    assert(!secondAccessHit);
    assert(statistics.getMemoryWrites() == 1);
    assert(statistics.getMemoryReads() == 1);

    std::cout
        << "PASS: no-write-allocate bypasses cache\n";
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

    std::cout << "\nAll cache tests passed.\n";

    return 0;
}