#include "ArchitectureComparisonRunner.hpp"

#include <cassert>
#include <iostream>
#include <vector>

int main() {
    std::vector<MemoryAccess> accesses;

    for (int repeat = 0; repeat < 100; ++repeat) {
        accesses.push_back({
            AccessType::Read,
            0x0000
        });

        accesses.push_back({
            AccessType::Read,
            0x0010
        });
    }

    const ArchitectureComparisonResult result =
        ArchitectureComparisonRunner::run(
            accesses,

            64,
            16,
            1,

            32,
            1,

            128,
            2,

            4.0,
            1.0,
            8.0,
            100.0,

            ReplacementPolicy::LRU,
            WritePolicy::WriteThrough,
            WriteMissPolicy::WriteAllocate
        );

    assert(result.singleCacheAverageCost >= 0.0);
    assert(result.hierarchyAverageTime >= 0.0);

    assert(
        result.singleCacheHitRate >= 0.0 &&
        result.singleCacheHitRate <= 100.0
    );

    assert(
        result.hierarchyGlobalHitRate >= 0.0 &&
        result.hierarchyGlobalHitRate <= 100.0
    );

    std::cout
        << "Architecture comparison tests passed.\n";

    return 0;
}