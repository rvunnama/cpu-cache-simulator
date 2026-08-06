#ifndef MISS_CLASSIFIER_HPP
#define MISS_CLASSIFIER_HPP

#include "MissType.hpp"

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <unordered_set>

class MissClassifier {
public:
    explicit MissClassifier(std::size_t totalCacheLines);

    MissType observeAccess(
        std::uint64_t blockAddress,
        bool wasMiss,
        bool bypassedCache
    );

private:
    using LruList = std::list<std::uint64_t>;
    using LruIterator = LruList::iterator;

    std::size_t capacity_;

    std::unordered_set<std::uint64_t> seenBlocks_;

    LruList shadowLru_;

    std::unordered_map<
        std::uint64_t,
        LruIterator
    > shadowLocations_;

    void updateShadowCache(
        std::uint64_t blockAddress
    );
};

#endif