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

private:
    std::size_t l1Hits_ = 0;
    std::size_t l1Misses_ = 0;
    std::size_t l2Hits_ = 0;
    std::size_t l2Misses_ = 0;
    std::size_t memoryAccesses_ = 0;
};

#endif