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

private:
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};

#endif