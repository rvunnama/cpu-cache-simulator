#include "CacheStatistics.hpp"

#include <iomanip>
#include <iostream>

void CacheStatistics::recordHit() {
    ++hits_;
}

void CacheStatistics::recordMiss() {
    ++misses_;
}

void CacheStatistics::recordMemoryRead() {
    ++memoryReads_;
}

void CacheStatistics::recordMemoryWrite() {
    ++memoryWrites_;
}

void CacheStatistics::recordDirtyEviction() {
    ++dirtyEvictions_;
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

std::size_t CacheStatistics::getMemoryReads() const {
    return memoryReads_;
}

std::size_t CacheStatistics::getMemoryWrites() const {
    return memoryWrites_;
}

std::size_t CacheStatistics::getDirtyEvictions() const {
    return dirtyEvictions_;
}

double CacheStatistics::calculateAmat(
    double cacheAccessTime,
    double memoryPenalty
) const {
    const double missRateFraction =
        getMissRate() / 100.0;

    return cacheAccessTime +
           missRateFraction * memoryPenalty;
}

double CacheStatistics::calculateAverageAccessCost(
    double cacheAccessTime,
    double memoryReadPenalty,
    double memoryWritePenalty
) const {
    const std::size_t totalAccesses =
        getTotalAccesses();

    if (totalAccesses == 0) {
        return 0.0;
    }

    const double totalMemoryCost =
        static_cast<double>(memoryReads_) *
            memoryReadPenalty +
        static_cast<double>(memoryWrites_) *
            memoryWritePenalty;

    return cacheAccessTime +
           totalMemoryCost /
               static_cast<double>(totalAccesses);
}

void CacheStatistics::recordMissType(
    MissType type
) {
    switch (type) {
        case MissType::Compulsory:
            ++compulsoryMisses_;
            break;

        case MissType::Conflict:
            ++conflictMisses_;
            break;

        case MissType::Capacity:
            ++capacityMisses_;
            break;

        case MissType::Bypass:
            ++bypassMisses_;
            break;

        case MissType::None:
            break;
    }
}

std::size_t
CacheStatistics::getCompulsoryMisses() const {
    return compulsoryMisses_;
}

std::size_t
CacheStatistics::getConflictMisses() const {
    return conflictMisses_;
}

std::size_t
CacheStatistics::getCapacityMisses() const {
    return capacityMisses_;
}

std::size_t
CacheStatistics::getBypassMisses() const {
    return bypassMisses_;
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

    std::cout
        << "Memory reads: "
        << memoryReads_
        << '\n';

    std::cout
        << "Memory writes: "
        << memoryWrites_
        << '\n';

    std::cout
        << "Dirty evictions: "
        << dirtyEvictions_
        << '\n';

    std::cout << "Compulsory misses: "
        << getCompulsoryMisses()
        << '\n';

    std::cout << "Conflict misses: "
        << getConflictMisses()
        << '\n';

    std::cout << "Capacity misses: "
        << getCapacityMisses()
        << '\n';

    std::cout << "Bypass misses: "
        << getBypassMisses()
        << '\n';
}