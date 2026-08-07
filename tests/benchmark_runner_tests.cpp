#include "BenchmarkRunner.hpp"

#include <cassert>
#include <iostream>
#include <vector>

int main() {
    const std::vector<MemoryAccess> accesses = {
        {AccessType::Read, 0x0000},
        {AccessType::Read, 0x0010},
        {AccessType::Read, 0x0000},
        {AccessType::Read, 0x0010}
    };

    const std::vector<BenchmarkResult> results =
        BenchmarkRunner::run(
            accesses,
            1.0,
            100.0,
            50.0
        );

    assert(!results.empty());

    for (std::size_t index = 1;
         index < results.size();
         ++index) {

        assert(
            results[index - 1].averageAccessCost <=
            results[index].averageAccessCost
        );
    }

    bool found64ByteCache = false;
    bool found128ByteCache = false;
    bool found256ByteCache = false;
    bool found512ByteCache = false;

    for (const BenchmarkResult& result : results) {
        if (result.cacheSize == 64) {
            found64ByteCache = true;
        }

        if (result.cacheSize == 128) {
            found128ByteCache = true;
        }

        if (result.cacheSize == 256) {
            found256ByteCache = true;
        }

        if (result.cacheSize == 512) {
            found512ByteCache = true;
        }

        assert(result.blockSize > 0);

        assert(
            result.cacheSize %
            result.blockSize == 0
        );

        const std::size_t totalLines =
            result.cacheSize /
            result.blockSize;

        assert(
            result.associativity <=
            totalLines
        );

        assert(
            totalLines %
            result.associativity == 0
        );

        assert(
            result.compulsoryMisses +
            result.conflictMisses +
            result.capacityMisses +
            result.bypassMisses ==
            result.misses
        );
    }

    assert(found64ByteCache);
    assert(found128ByteCache);
    assert(found256ByteCache);
    assert(found512ByteCache);

    std::cout
        << "All benchmark runner tests passed.\n";

    return 0;
}