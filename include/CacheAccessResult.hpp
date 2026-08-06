#ifndef CACHE_ACCESS_RESULT_HPP
#define CACHE_ACCESS_RESULT_HPP

#include <cstdint>

struct CacheAccessResult {
    bool hit = false;
    bool bypassed = false;

    bool evictionOccurred = false;
    bool dirtyEviction = false;

    std::uint64_t evictedBlockAddress = 0;
};

#endif