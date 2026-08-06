#ifndef CACHE_HIERARCHY_HPP
#define CACHE_HIERARCHY_HPP

#include "HierarchyAccessResult.hpp"
#include "HierarchyStatistics.hpp"
#include "MemoryAccess.hpp"
#include "SetAssociativeCache.hpp"

class CacheHierarchy {
public:
    CacheHierarchy(
        SetAssociativeCache l1Cache,
        SetAssociativeCache l2Cache
    );

    HierarchyAccessResult access(
        const MemoryAccess& access
    );

    const SetAssociativeCache& getL1() const;
    const SetAssociativeCache& getL2() const;

    const HierarchyStatistics&
    getStatistics() const;

    void printStatistics() const;

private:
    SetAssociativeCache l1_;
    SetAssociativeCache l2_;

    HierarchyStatistics statistics_;
};

#endif