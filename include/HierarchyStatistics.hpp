#ifndef HIERARCHY_STATISTICS_HPP
#define HIERARCHY_STATISTICS_HPP

#include <cstddef>

class HierarchyStatistics {
public:
    void recordL1Hit();
    void recordL1Miss();
    void recordL2Hit();
    void recordL2Miss();
    void recordMemoryAccess();

    std::size_t getL1Hits() const;
    std::size_t getL1Misses() const;
    std::size_t getL2Hits() const;
    std::size_t getL2Misses() const;
    std::size_t getMemoryAccesses() const;

    void printReport() const;

    void recordL1Eviction();
    void recordL1DirtyWriteBack();
    void recordL2Eviction();
    void recordL2DirtyWriteBack();

    std::size_t getL1Evictions() const;
    std::size_t getL1DirtyWriteBacks() const;
    std::size_t getL2Evictions() const;
    std::size_t getL2DirtyWriteBacks() const;

    void recordL1Invalidation();
    void recordDirtyL1Invalidation();

    std::size_t getL1Invalidations() const;
    std::size_t getDirtyL1Invalidations() const;

private:
    std::size_t l1Hits_ = 0;
    std::size_t l1Misses_ = 0;
    std::size_t l2Hits_ = 0;
    std::size_t l2Misses_ = 0;
    std::size_t memoryAccesses_ = 0;
    std::size_t l1Evictions_ = 0;
    std::size_t l1DirtyWriteBacks_ = 0;
    std::size_t l2Evictions_ = 0;
    std::size_t l2DirtyWriteBacks_ = 0;
    std::size_t l1Invalidations_ = 0;
    std::size_t dirtyL1Invalidations_ = 0;
};

#endif