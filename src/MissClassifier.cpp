#include "MissClassifier.hpp"

#include <stdexcept>

MissClassifier::MissClassifier(
    std::size_t totalCacheLines
)
    : capacity_(totalCacheLines) {

    if (capacity_ == 0) {
        throw std::invalid_argument(
            "Miss-classifier capacity must be "
            "greater than zero."
        );
    }
}

MissType MissClassifier::observeAccess(
    std::uint64_t blockAddress,
    bool wasMiss,
    bool bypassedCache
) {
    const bool firstAccess =
        seenBlocks_.find(blockAddress) ==
        seenBlocks_.end();

    const bool shadowHit =
        shadowLocations_.find(blockAddress) !=
        shadowLocations_.end();

    MissType missType = MissType::None;

    if (wasMiss) {
        if (bypassedCache) {
            missType = MissType::Bypass;
        } else if (firstAccess) {
            missType = MissType::Compulsory;
        } else if (shadowHit) {
            missType = MissType::Conflict;
        } else {
            missType = MissType::Capacity;
        }
    }

    seenBlocks_.insert(blockAddress);
    updateShadowCache(blockAddress);

    return missType;
}

void MissClassifier::updateShadowCache(
    std::uint64_t blockAddress
) {
    const auto existing =
        shadowLocations_.find(blockAddress);

    if (existing != shadowLocations_.end()) {
        shadowLru_.erase(existing->second);
    }

    shadowLru_.push_front(blockAddress);
    shadowLocations_[blockAddress] =
        shadowLru_.begin();

    if (shadowLru_.size() > capacity_) {
        const std::uint64_t evictedBlock =
            shadowLru_.back();

        shadowLru_.pop_back();
        shadowLocations_.erase(evictedBlock);
    }
}