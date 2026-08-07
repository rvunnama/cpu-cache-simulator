#include "HierarchyStatistics.hpp"

#include <iostream>

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
}