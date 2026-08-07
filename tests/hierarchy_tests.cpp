#include "CacheHierarchy.hpp"
#include "ReplacementPolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "WritePolicy.hpp"

#include <cassert>
#include <iostream>
#include <utility>
#include <cmath>

void testDirtyL1EvictionWritesBackToL2() {
    SetAssociativeCache l1(
        16,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    SetAssociativeCache l2(
        64,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    CacheHierarchy hierarchy(
        std::move(l1),
        std::move(l2)
    );

    hierarchy.access({
        AccessType::Write,
        0x0000
    });

    hierarchy.access({
        AccessType::Read,
        0x0010
    });

    const HierarchyStatistics& statistics =
        hierarchy.getStatistics();

    assert(statistics.getL1Evictions() == 1);

    assert(
        statistics.getL1DirtyWriteBacks() == 1
    );

    const SetAssociativeCache& l2Cache =
        hierarchy.getL2();

    bool dirtyBlockFound = false;

    for (const CacheSet& set : l2Cache.getSets()) {
        for (const CacheLine& line : set.getLines()) {
            if (
                line.valid &&
                line.blockAddress == 0 &&
                line.dirty
            ) {
                dirtyBlockFound = true;
            }
        }
    }

    assert(dirtyBlockFound);

    std::cout
        << "PASS: dirty L1 eviction writes back to L2\n";
}

void testL2EvictionInvalidatesL1() {
    SetAssociativeCache l1(
        32,
        16,
        2,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    SetAssociativeCache l2(
        16,
        16,
        1,
        ReplacementPolicy::LRU,
        WritePolicy::WriteBack,
        WriteMissPolicy::WriteAllocate
    );

    CacheHierarchy hierarchy(
        std::move(l1),
        std::move(l2)
    );

    hierarchy.access({
        AccessType::Read,
        0x0000
    });

    hierarchy.access({
        AccessType::Read,
        0x0010
    });

    const bool blockZeroStillInL1 =
        hierarchy.getL1().hasBlock(0);

    assert(!blockZeroStillInL1);

    const HierarchyStatistics& statistics =
        hierarchy.getStatistics();

    assert(statistics.getL2Evictions() == 1);
    assert(statistics.getL1Invalidations() == 1);

    std::cout
        << "PASS: L2 eviction invalidates L1 copy\n";
}

void testHierarchyPerformanceMetrics() {
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

    hierarchy.access({
        AccessType::Read,
        0x0000
    });

    hierarchy.access({
        AccessType::Read,
        0x0000
    });

    hierarchy.access({
        AccessType::Read,
        0x0010
    });

    hierarchy.access({
        AccessType::Read,
        0x0000
    });

    const HierarchyStatistics& statistics =
        hierarchy.getStatistics();

    assert(statistics.getL1Hits() == 1);
    assert(statistics.getL1Misses() == 3);

    assert(statistics.getL2Hits() == 1);
    assert(statistics.getL2Misses() == 2);

    assert(
        std::abs(
            statistics.getL1HitRate() - 25.0
        ) < 0.0001
    );

    assert(
        std::abs(
            statistics.getL2LocalHitRate() -
            33.333333
        ) < 0.001
    );

    assert(
        std::abs(
            statistics.getGlobalHitRate() -
            50.0
        ) < 0.0001
    );

    const double averageTime =
        statistics.calculateAverageAccessTime(
            1.0,
            10.0,
            100.0
        );

    // Access costs:
    // memory = 111
    // L1 hit = 1
    // memory = 111
    // L2 hit = 11
    //
    // Total = 234
    // Average = 58.5 ns

    assert(
        std::abs(
            averageTime - 58.5
        ) < 0.0001
    );

    std::cout
        << "PASS: hierarchy performance metrics\n";
}

int main() {
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

    const MemoryAccess firstAccess {
        AccessType::Read,
        0x0000
    };

    const MemoryAccess secondBlock {
        AccessType::Read,
        0x0010
    };

    const HierarchyAccessResult first =
        hierarchy.access(firstAccess);

    assert(!first.l1Hit);
    assert(!first.l2Hit);
    assert(first.memoryAccess);

    const HierarchyAccessResult second =
        hierarchy.access(firstAccess);

    assert(second.l1Hit);
    assert(!second.l2Hit);
    assert(!second.memoryAccess);

    hierarchy.access(secondBlock);

    const HierarchyAccessResult l2Result =
        hierarchy.access(firstAccess);

    assert(!l2Result.l1Hit);
    assert(l2Result.l2Hit);
    assert(!l2Result.memoryAccess);

    const HierarchyStatistics& statistics =
        hierarchy.getStatistics();

    assert(statistics.getL1Hits() == 1);
    assert(statistics.getL1Misses() == 3);
    assert(statistics.getL2Hits() == 1);
    assert(statistics.getL2Misses() == 2);
    assert(statistics.getMemoryAccesses() == 2);

    testDirtyL1EvictionWritesBackToL2();

    testL2EvictionInvalidatesL1();

    testHierarchyPerformanceMetrics();

    std::cout
        << "All cache hierarchy tests passed.\n";

    return 0;
}