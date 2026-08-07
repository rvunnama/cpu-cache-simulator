#include "HierarchyStatistics.hpp"

#include <iostream>
#include <iomanip>

void HierarchyStatistics::recordL1Hit() {
    ++l1Hits_;
}

void HierarchyStatistics::recordL1Miss() {
    ++l1Misses_;
}

void HierarchyStatistics::recordL2Hit() {
    ++l2Hits_;
}

void HierarchyStatistics::recordL2Miss() {
    ++l2Misses_;
}

void HierarchyStatistics::recordMemoryAccess() {
    ++memoryAccesses_;
}

std::size_t HierarchyStatistics::getL1Hits() const {
    return l1Hits_;
}

std::size_t HierarchyStatistics::getL1Misses() const {
    return l1Misses_;
}

std::size_t HierarchyStatistics::getL2Hits() const {
    return l2Hits_;
}

std::size_t HierarchyStatistics::getL2Misses() const {
    return l2Misses_;
}

std::size_t
HierarchyStatistics::getMemoryAccesses() const {
    return memoryAccesses_;
}

void HierarchyStatistics::recordL1Eviction() {
    ++l1Evictions_;
}

void HierarchyStatistics::recordL1DirtyWriteBack() {
    ++l1DirtyWriteBacks_;
}

void HierarchyStatistics::recordL2Eviction() {
    ++l2Evictions_;
}

void HierarchyStatistics::recordL2DirtyWriteBack() {
    ++l2DirtyWriteBacks_;
}

std::size_t HierarchyStatistics::getL1Evictions() const {
    return l1Evictions_;
}

std::size_t
HierarchyStatistics::getL1DirtyWriteBacks() const {
    return l1DirtyWriteBacks_;
}

std::size_t HierarchyStatistics::getL2Evictions() const {
    return l2Evictions_;
}

std::size_t
HierarchyStatistics::getL2DirtyWriteBacks() const {
    return l2DirtyWriteBacks_;
}

void HierarchyStatistics::recordL1Invalidation() {
    ++l1Invalidations_;
}

void HierarchyStatistics::recordDirtyL1Invalidation() {
    ++dirtyL1Invalidations_;
}

std::size_t
HierarchyStatistics::getL1Invalidations() const {
    return l1Invalidations_;
}

std::size_t
HierarchyStatistics::getDirtyL1Invalidations() const {
    return dirtyL1Invalidations_;
}

double HierarchyStatistics::getL1HitRate() const {
    const std::size_t total =
        l1Hits_ + l1Misses_;

    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(l1Hits_) /
           static_cast<double>(total) *
           100.0;
}

double HierarchyStatistics::getL2LocalHitRate() const {
    const std::size_t total =
        l2Hits_ + l2Misses_;

    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(l2Hits_) /
           static_cast<double>(total) *
           100.0;
}

double HierarchyStatistics::getGlobalHitRate() const {
    const std::size_t totalCpuAccesses =
        l1Hits_ + l1Misses_;

    if (totalCpuAccesses == 0) {
        return 0.0;
    }

    const std::size_t cacheHits =
        l1Hits_ + l2Hits_;

    return static_cast<double>(cacheHits) /
           static_cast<double>(totalCpuAccesses) *
           100.0;
}

double HierarchyStatistics::calculateAverageAccessTime(
    double l1Latency,
    double l2Latency,
    double memoryLatency
) const {
    const std::size_t totalCpuAccesses =
        l1Hits_ + l1Misses_;

    if (totalCpuAccesses == 0) {
        return 0.0;
    }

    const double totalTime =
        static_cast<double>(l1Hits_) *
            l1Latency +

        static_cast<double>(l2Hits_) *
            (l1Latency + l2Latency) +

        static_cast<double>(l2Misses_) *
            (
                l1Latency +
                l2Latency +
                memoryLatency
            );

    return totalTime /
           static_cast<double>(totalCpuAccesses);
}

void HierarchyStatistics::printReport() const {
    std::cout << "\nCache Hierarchy Statistics\n";
    std::cout << "--------------------------\n";
    std::cout << "L1 hits: "
              << l1Hits_
              << '\n';
    std::cout << "L1 misses: "
              << l1Misses_
              << '\n';
    std::cout << "L2 hits: "
              << l2Hits_
              << '\n';
    std::cout << "L2 misses: "
              << l2Misses_
              << '\n';
    std::cout << "Main-memory accesses: "
              << memoryAccesses_
              << '\n';
    std::cout << "L1 evictions: "
              << l1Evictions_
              << '\n';

    std::cout << "L1 dirty write-backs to L2: "
              << l1DirtyWriteBacks_
              << '\n';

    std::cout << "L2 evictions: "
              << l2Evictions_
              << '\n';

    std::cout << "L2 dirty write-backs to memory: "
              << l2DirtyWriteBacks_
              << '\n';

    std::cout << "L1 invalidations caused by L2: "
              << l1Invalidations_
              << '\n';

    std::cout << "Dirty L1 invalidations: "
              << dirtyL1Invalidations_
              << '\n';
    
    std::cout << std::fixed
              << std::setprecision(2);

    std::cout << "L1 hit rate: "
              << getL1HitRate()
              << "%\n";

    std::cout << "L2 local hit rate: "
              << getL2LocalHitRate()
              << "%\n";

    std::cout << "Global cache hit rate: "
              << getGlobalHitRate()
              << "%\n";
}