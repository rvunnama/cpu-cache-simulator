#ifndef SET_ASSOCIATIVE_CACHE_HPP
#define SET_ASSOCIATIVE_CACHE_HPP

#include "CacheSet.hpp"
#include "CacheStatistics.hpp"
#include "ReplacementPolicy.hpp"
#include "MemoryAccess.hpp"
#include "WritePolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "MissClassifier.hpp"
#include "CacheAccessResult.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class SetAssociativeCache {
public:
    SetAssociativeCache(
        std::size_t cacheSize,
        std::size_t blockSize,
        std::size_t associativity,
        ReplacementPolicy replacementPolicy,
        WritePolicy writePolicy,
        WriteMissPolicy writeMissPolicy
    );

    CacheAccessResult access(
        const MemoryAccess& access
    );

    void printStatistics() const;

    const std::vector<CacheSet>& getSets() const;

    const CacheStatistics& getStatistics() const;

private:
    std::size_t cacheSize_;
    std::size_t blockSize_;
    std::size_t associativity_;
    std::size_t numberOfLines_;
    std::size_t numberOfSets_;

    ReplacementPolicy replacementPolicy_;

    WritePolicy writePolicy_;

    WriteMissPolicy writeMissPolicy_;

    std::vector<CacheSet> sets_;
    CacheStatistics statistics_;

    std::size_t accessCounter_ = 0;

    MissClassifier missClassifier_;

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