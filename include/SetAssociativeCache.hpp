#ifndef SET_ASSOCIATIVE_CACHE_HPP
#define SET_ASSOCIATIVE_CACHE_HPP

#include "CacheSet.hpp"
#include "CacheStatistics.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class SetAssociativeCache {
public:
    SetAssociativeCache(
        std::size_t cacheSize,
        std::size_t blockSize,
        std::size_t associativity
    );

    bool access(std::uint64_t address);

    void printStatistics() const;

    const std::vector<CacheSet>& getSets() const;

private:
    std::size_t cacheSize_;
    std::size_t blockSize_;
    std::size_t associativity_;
    std::size_t numberOfLines_;
    std::size_t numberOfSets_;

    std::vector<CacheSet> sets_;
    CacheStatistics statistics_;

    std::uint64_t calculateBlockAddress(
        std::uint64_t address
    ) const;

    std::size_t calculateSetIndex(
        std::uint64_t blockAddress
    ) const;

    std::uint64_t calculateTag(
        std::uint64_t blockAddress
    ) const;
};

#endif