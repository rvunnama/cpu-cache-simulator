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
}