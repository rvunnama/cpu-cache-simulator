#include "CacheHierarchy.hpp"
#include "ReplacementPolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "WritePolicy.hpp"

#include <cassert>
#include <iostream>
#include <utility>

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

    std::cout
        << "All cache hierarchy tests passed.\n";

    return 0;
}