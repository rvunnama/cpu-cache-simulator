#ifndef CACHE_STATISTICS_HPP
#define CACHE_STATISTICS_HPP

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

    double calculateAmat(
        double cacheAccessTime,
        double memoryPenalty
    ) const;

    double calculateAverageAccessCost(
        double cacheAccessTime,
        double memoryReadPenalty,
        double memoryWritePenalty
    ) const;

private:
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
    std::size_t memoryReads_ = 0;
    std::size_t memoryWrites_ = 0;
    std::size_t dirtyEvictions_ = 0;
};

#endif