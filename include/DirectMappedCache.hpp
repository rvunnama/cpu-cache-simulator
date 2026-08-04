#ifndef DIRECT_MAPPED_CACHE_HPP
#define DIRECT_MAPPED_CACHE_HPP

#include "CacheLine.hpp"
#include "CacheStatistics.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class DirectMappedCache {
public:
    DirectMappedCache(
        std::size_t cacheSize,
        std::size_t blockSize
    );

    bool access(std::uint64_t address);

    void printStatistics() const;

private:
    std::size_t cacheSize_;
    std::size_t blockSize_;
    std::size_t numberOfLines_;

    std::vector<CacheLine> lines_;
    CacheStatistics statistics_;

    std::uint64_t calculateBlockAddress(
        std::uint64_t address
    ) const;

    std::size_t calculateIndex(
        std::uint64_t blockAddress
    ) const;

    std::uint64_t calculateTag(
        std::uint64_t blockAddress
    ) const;
};

#endif