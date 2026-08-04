#include "CacheStatistics.hpp"

#include <iomanip>
#include <iostream>

void CacheStatistics::recordHit() {
    ++hits_;
}

void CacheStatistics::recordMiss() {
    ++misses_;
}

std::size_t CacheStatistics::getHits() const {
    return hits_;
}

std::size_t CacheStatistics::getMisses() const {
    return misses_;
}

std::size_t CacheStatistics::getTotalAccesses() const {
    return hits_ + misses_;
}

double CacheStatistics::getHitRate() const {
    const std::size_t total = getTotalAccesses();

    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(hits_) /
           static_cast<double>(total) *
           100.0;
}

double CacheStatistics::getMissRate() const {
    const std::size_t total = getTotalAccesses();

    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(misses_) /
           static_cast<double>(total) *
           100.0;
}

void CacheStatistics::printReport() const {
    std::cout << "\nStatistics\n";
    std::cout << "----------\n";
    std::cout << "Total accesses: " << getTotalAccesses() << '\n';
    std::cout << "Hits: " << getHits() << '\n';
    std::cout << "Misses: " << getMisses() << '\n';

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Hit rate: " << getHitRate() << "%\n";
    std::cout << "Miss rate: " << getMissRate() << "%\n";
}