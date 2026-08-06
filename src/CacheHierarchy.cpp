#include "CacheHierarchy.hpp"

#include <utility>

CacheHierarchy::CacheHierarchy(
    SetAssociativeCache l1Cache,
    SetAssociativeCache l2Cache
)
    : l1_(std::move(l1Cache)),
      l2_(std::move(l2Cache)) {
}

HierarchyAccessResult CacheHierarchy::access(
    const MemoryAccess& access
) {
    if (l1_.contains(access.address)) {
        statistics_.recordL1Hit();

        return {
            true,
            false,
            false
        };
    }

    statistics_.recordL1Miss();

    if (l2_.contains(access.address)) {
        statistics_.recordL2Hit();

        l1_.insert(access);

        return {
            false,
            true,
            false
        };
    }

    statistics_.recordL2Miss();
    statistics_.recordMemoryAccess();

    l2_.insert(access);
    l1_.insert(access);

    return {
        false,
        false,
        true
    };
}

const SetAssociativeCache&
CacheHierarchy::getL1() const {
    return l1_;
}

const SetAssociativeCache&
CacheHierarchy::getL2() const {
    return l2_;
}

const HierarchyStatistics&
CacheHierarchy::getStatistics() const {
    return statistics_;
}

void CacheHierarchy::printStatistics() const {
    statistics_.printReport();
}