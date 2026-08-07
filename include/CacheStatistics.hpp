#ifndef CACHE_STATISTICS_HPP
#define CACHE_STATISTICS_HPP

#include "MissType.hpp"
#include "MissClassifier.hpp"

#include <cstddef>

class CacheStatistics {
public:
    void recordHit();
    void recordMiss();

    std::size_t getHits() const;
    std::size_t getMisses() const;
    std::size_t getTotalAccesses() const;

    double getHitRate() const;
    double getMissRate() const;

    void printReport() const;

    void recordMemoryRead();

    void recordMemoryWrite();

    void recordDirtyEviction();

    std::size_t getMemoryReads() const;

    std::size_t getMemoryWrites() const;

    std::size_t getDirtyEvictions() const;

    std::size_t getCompulsoryMisses() const;

    std::size_t getConflictMisses() const;

    std::size_t getCapacityMisses() const;

    std::size_t getBypassMisses() const;

    double calculateAmat(
        double cacheAccessTime,
        double memoryPenalty
    ) const;

    double calculateAverageAccessCost(
        double cacheAccessTime,
        double memoryReadPenalty,
        double memoryWritePenalty
    ) const;

    void recordMissType(MissType type);

    void recordPrefetch();
    std::size_t getPrefetches() const;

private:
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
    std::size_t memoryReads_ = 0;
    std::size_t memoryWrites_ = 0;
    std::size_t dirtyEvictions_ = 0;
    std::size_t compulsoryMisses_ = 0;
    std::size_t conflictMisses_ = 0;
    std::size_t capacityMisses_ = 0;
    std::size_t bypassMisses_ = 0;
    std::size_t prefetches_ = 0;
};

#endif