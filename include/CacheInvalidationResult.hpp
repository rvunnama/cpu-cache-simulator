#ifndef CACHE_INVALIDATION_RESULT_HPP
#define CACHE_INVALIDATION_RESULT_HPP

struct CacheInvalidationResult {
    bool found = false;
    bool dirty = false;
};

#endif