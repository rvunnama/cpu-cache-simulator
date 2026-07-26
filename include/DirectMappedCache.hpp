#ifndef DIRECT_MAPPED_CACHE_HPP
#define DIRECT_MAPPED_CACHE_HPP

#include "CacheLine.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class DirectMappedCache {
public:
    DirectMappedCache(std::size_t cacheSize, std::size_t blockSize);

    bool access(std::uint64_t address);

    void printStatistics() const;

private:
    std::size_t cacheSize_;
    std::size_t blockSize_;
    std::size_t numberOfLines_;

    std::vector<CacheLine> lines_;

    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};

#endif