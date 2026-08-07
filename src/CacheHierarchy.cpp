#include "CacheHierarchy.hpp"
#include "CacheInvalidationResult.hpp"

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

        const CacheAccessResult l1Result =
            l1_.insert(access);

        handleL1InsertionResult(l1Result);

        return {
            false,
            true,
            false
        };
    }

    statistics_.recordL2Miss();
    statistics_.recordMemoryAccess();

    const CacheAccessResult l2Result =
        l2_.insert(access);

    handleL2InsertionResult(l2Result);

    const CacheAccessResult l1Result =
        l1_.insert(access);

    handleL1InsertionResult(l1Result);

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

void CacheHierarchy::handleL1InsertionResult(
    const CacheAccessResult& result
) {
    if (!result.evictionOccurred) {
        return;
    }

    statistics_.recordL1Eviction();

    if (!result.dirtyEviction) {
        return;
    }

    statistics_.recordL1DirtyWriteBack();

    const CacheAccessResult l2Result =
        l2_.writeBackBlock(
            result.evictedBlockAddress
        );

    handleL2InsertionResult(l2Result);
}

void CacheHierarchy::handleL2InsertionResult(
    const CacheAccessResult& result
) {
    if (!result.evictionOccurred) {
        return;
    }

    statistics_.recordL2Eviction();

    const CacheInvalidationResult l1Invalidation =
        l1_.invalidateBlock(
            result.evictedBlockAddress
        );

    if (l1Invalidation.found) {
        statistics_.recordL1Invalidation();
    }

    const bool newestDataWasDirty =
        result.dirtyEviction ||
        l1Invalidation.dirty;

    if (l1Invalidation.dirty) {
        statistics_.recordDirtyL1Invalidation();
    }

    if (newestDataWasDirty) {
        statistics_.recordL2DirtyWriteBack();
        statistics_.recordMemoryAccess();
    }
}