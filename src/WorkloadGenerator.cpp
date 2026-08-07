#include "WorkloadGenerator.hpp"

#include <random>
#include <stdexcept>
#include <vector>

std::vector<MemoryAccess> WorkloadGenerator::generate(
    const std::string& pattern,
    std::size_t accessCount,
    std::uint64_t startAddress,
    std::uint64_t stride,
    std::size_t workingSetSize,
    unsigned int seed
) {
    if (accessCount == 0) {
        throw std::invalid_argument(
            "Access count must be greater than zero."
        );
    }

    if (stride == 0) {
        throw std::invalid_argument(
            "Stride must be greater than zero."
        );
    }

    if (pattern == "sequential") {
        return generateSequential(
            accessCount,
            startAddress,
            stride
        );
    }

    if (pattern == "loop") {
        return generateLoop(
            accessCount,
            startAddress,
            stride,
            workingSetSize
        );
    }

    if (pattern == "stride") {
        return generateStride(
            accessCount,
            startAddress,
            stride
        );
    }

    if (pattern == "random") {
        return generateRandom(
            accessCount,
            startAddress,
            stride,
            workingSetSize,
            seed
        );
    }

    throw std::invalid_argument(
        "Unsupported workload pattern: " + pattern
    );
}

std::vector<MemoryAccess>
WorkloadGenerator::generateSequential(
    std::size_t accessCount,
    std::uint64_t startAddress,
    std::uint64_t stride
) {
    std::vector<MemoryAccess> accesses;
    accesses.reserve(accessCount);

    for (
        std::size_t index = 0;
        index < accessCount;
        ++index
    ) {
        accesses.push_back({
            AccessType::Read,
            startAddress +
                static_cast<std::uint64_t>(index) *
                stride
        });
    }

    return accesses;
}

std::vector<MemoryAccess>
WorkloadGenerator::generateLoop(
    std::size_t accessCount,
    std::uint64_t startAddress,
    std::uint64_t stride,
    std::size_t workingSetSize
) {
    if (workingSetSize == 0) {
        throw std::invalid_argument(
            "Working-set size must be greater than zero."
        );
    }

    std::vector<MemoryAccess> accesses;
    accesses.reserve(accessCount);

    for (
        std::size_t index = 0;
        index < accessCount;
        ++index
    ) {
        const std::size_t position =
            index % workingSetSize;

        accesses.push_back({
            AccessType::Read,
            startAddress +
                static_cast<std::uint64_t>(position) *
                stride
        });
    }

    return accesses;
}

std::vector<MemoryAccess>
WorkloadGenerator::generateStride(
    std::size_t accessCount,
    std::uint64_t startAddress,
    std::uint64_t stride
) {
    return generateSequential(
        accessCount,
        startAddress,
        stride
    );
}

std::vector<MemoryAccess>
WorkloadGenerator::generateRandom(
    std::size_t accessCount,
    std::uint64_t startAddress,
    std::uint64_t stride,
    std::size_t workingSetSize,
    unsigned int seed
) {
    if (workingSetSize == 0) {
        throw std::invalid_argument(
            "Working-set size must be greater than zero."
        );
    }

    std::mt19937 generator(seed);

    std::uniform_int_distribution<std::size_t>
        distribution(
            0,
            workingSetSize - 1
        );

    std::vector<MemoryAccess> accesses;
    accesses.reserve(accessCount);

    for (
        std::size_t index = 0;
        index < accessCount;
        ++index
    ) {
        const std::size_t position =
            distribution(generator);

        accesses.push_back({
            AccessType::Read,
            startAddress +
                static_cast<std::uint64_t>(position) *
                stride
        });
    }

    return accesses;
}