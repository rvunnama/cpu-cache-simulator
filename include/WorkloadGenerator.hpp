#ifndef WORKLOAD_GENERATOR_HPP
#define WORKLOAD_GENERATOR_HPP

#include "MemoryAccess.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class WorkloadGenerator {
public:
    static std::vector<MemoryAccess> generate(
        const std::string& pattern,
        std::size_t accessCount,
        std::uint64_t startAddress,
        std::uint64_t stride,
        std::size_t workingSetSize,
        unsigned int seed
    );

private:
    static std::vector<MemoryAccess> generateSequential(
        std::size_t accessCount,
        std::uint64_t startAddress,
        std::uint64_t stride
    );

    static std::vector<MemoryAccess> generateLoop(
        std::size_t accessCount,
        std::uint64_t startAddress,
        std::uint64_t stride,
        std::size_t workingSetSize
    );

    static std::vector<MemoryAccess> generateStride(
        std::size_t accessCount,
        std::uint64_t startAddress,
        std::uint64_t stride
    );

    static std::vector<MemoryAccess> generateRandom(
        std::size_t accessCount,
        std::uint64_t startAddress,
        std::uint64_t stride,
        std::size_t workingSetSize,
        unsigned int seed
    );
};

#endif