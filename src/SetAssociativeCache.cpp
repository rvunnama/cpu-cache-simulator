#include "SetAssociativeCache.hpp"

#include <stdexcept>
#include <vector>

namespace {

std::size_t validateAndCalculateLineCount(
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    WritePolicy writePolicy,
    WriteMissPolicy writeMissPolicy
) {
    if (cacheSize == 0) {
        throw std::invalid_argument(
            "Cache size must be greater than zero."
        );
    }

    if (blockSize == 0) {
        throw std::invalid_argument(
            "Block size must be greater than zero."
        );
    }

    if (associativity == 0) {
        throw std::invalid_argument(
            "Associativity must be greater than zero."
        );
    }

    if (cacheSize % blockSize != 0) {
        throw std::invalid_argument(
            "Cache size must be divisible by block size."
        );
    }

    const std::size_t numberOfLines =
        cacheSize / blockSize;

    if (numberOfLines % associativity != 0) {
        throw std::invalid_argument(
            "Number of cache lines must be divisible "
            "by associativity."
        );
    }

    if (
        writePolicy == WritePolicy::WriteBack &&
        writeMissPolicy == WriteMissPolicy::NoWriteAllocate
    ) {
        throw std::invalid_argument(
            "Write-back must be used with write-allocate."
        );
    }

    return numberOfLines;
}

}  // namespace

SetAssociativeCache::SetAssociativeCache(
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    ReplacementPolicy replacementPolicy,
    WritePolicy writePolicy,
    WriteMissPolicy writeMissPolicy
)
    : cacheSize_(cacheSize),
      blockSize_(blockSize),
      associativity_(associativity),
      numberOfLines_(
        validateAndCalculateLineCount(
            cacheSize,
            blockSize,
            associativity,
            writePolicy,
            writeMissPolicy
        )
      ),
      numberOfSets_(numberOfLines_ / associativity_),
      replacementPolicy_(replacementPolicy),
      writePolicy_(writePolicy),
      writeMissPolicy_(writeMissPolicy),
      sets_(), 
      missClassifier_(numberOfLines_) {

    sets_.reserve(numberOfSets_);

    for (
        std::size_t index = 0;
        index < numberOfSets_;
        ++index
    ) {
        sets_.emplace_back(associativity_);
    }
}

CacheAccessResult SetAssociativeCache::access(
    const MemoryAccess& access
) {
    ++accessCounter_;

    const std::uint64_t blockAddress =
        calculateBlockAddress(access.address);

    const std::size_t setIndex =
        calculateSetIndex(blockAddress);

    const std::uint64_t tag =
        calculateTag(blockAddress);

    CacheSet& set = sets_.at(setIndex);
    std::vector<CacheLine>& lines = set.getLines();

    for (CacheLine& line : lines) {
        if (line.valid && line.tag == tag) {
            line.lastAccessOrder = accessCounter_;

            if (
                access.type == AccessType::Write &&
                writePolicy_ == WritePolicy::WriteBack
            ) {
                line.dirty = true;
            }

            if (
                access.type == AccessType::Write &&
                writePolicy_ == WritePolicy::WriteThrough
            ) {
                statistics_.recordMemoryWrite();
            }

            statistics_.recordHit();

            missClassifier_.observeAccess(
                blockAddress,
                false,
                false
            );

            return {
                true,   // hit
                false,  // bypassed
                false,  // eviction occurred
                false,  // dirty eviction
                0       // evicted block
            };
        }
    }

    statistics_.recordMiss();

    const bool bypassedCache =
        access.type == AccessType::Write &&
        writeMissPolicy_ ==
            WriteMissPolicy::NoWriteAllocate;

    const MissType missType =
        missClassifier_.observeAccess(
            blockAddress,
            true,
            bypassedCache
        );

    statistics_.recordMissType(missType);

    if (bypassedCache) {
        statistics_.recordMemoryWrite();

        return {
            false,
            true,
            false,
            false,
            0
        };
    }

    statistics_.recordMemoryRead();

    CacheLine& insertionLine =
        set.selectLineForInsertion(
            replacementPolicy_
        );

    const bool evictionOccurred =
        insertionLine.valid;

    const bool dirtyEviction =
        insertionLine.valid &&
        insertionLine.dirty &&
        writePolicy_ == WritePolicy::WriteBack;

    const std::uint64_t evictedBlockAddress =
        evictionOccurred
            ? insertionLine.blockAddress
            : 0;

    if (dirtyEviction) {
        statistics_.recordDirtyEviction();
        statistics_.recordMemoryWrite();
    }

    insertionLine.valid = true;

    insertionLine.dirty =
        access.type == AccessType::Write &&
        writePolicy_ == WritePolicy::WriteBack;

    insertionLine.tag = tag;
    insertionLine.blockAddress = blockAddress;
    insertionLine.insertionOrder = accessCounter_;
    insertionLine.lastAccessOrder = accessCounter_;

    if (
        access.type == AccessType::Write &&
        writePolicy_ == WritePolicy::WriteThrough
    ) {
        statistics_.recordMemoryWrite();
    }

    return {
        false,
        false,
        evictionOccurred,
        dirtyEviction,
        evictedBlockAddress
    };
}

std::uint64_t
SetAssociativeCache::calculateBlockAddress(
    std::uint64_t address
) const {
    return address / blockSize_;
}

std::size_t
SetAssociativeCache::calculateSetIndex(
    std::uint64_t blockAddress
) const {
    return static_cast<std::size_t>(
        blockAddress % numberOfSets_
    );
}

std::uint64_t
SetAssociativeCache::calculateTag(
    std::uint64_t blockAddress
) const {
    return blockAddress / numberOfSets_;
}

void SetAssociativeCache::printStatistics() const {
    statistics_.printReport();
}

const std::vector<CacheSet>&
SetAssociativeCache::getSets() const {
    return sets_;
}

const CacheStatistics&
SetAssociativeCache::getStatistics() const {
    return statistics_;
}

bool SetAssociativeCache::contains(
    std::uint64_t address
) {
    ++accessCounter_;

    const std::uint64_t blockAddress =
        calculateBlockAddress(address);

    const std::size_t setIndex =
        calculateSetIndex(blockAddress);

    const std::uint64_t tag =
        calculateTag(blockAddress);

    CacheSet& set = sets_.at(setIndex);
    std::vector<CacheLine>& lines = set.getLines();

    for (CacheLine& line : lines) {
        if (line.valid && line.tag == tag) {
            line.lastAccessOrder = accessCounter_;
            return true;
        }
    }

    return false;
}

CacheAccessResult SetAssociativeCache::insert(
    const MemoryAccess& access
) {
    ++accessCounter_;

    const std::uint64_t blockAddress =
        calculateBlockAddress(access.address);

    const std::size_t setIndex =
        calculateSetIndex(blockAddress);

    const std::uint64_t tag =
        calculateTag(blockAddress);

    CacheSet& set = sets_.at(setIndex);

    CacheLine& insertionLine =
        set.selectLineForInsertion(
            replacementPolicy_
        );

    const bool evictionOccurred =
        insertionLine.valid;

    const bool dirtyEviction =
        insertionLine.valid &&
        insertionLine.dirty;

    const std::uint64_t evictedBlockAddress =
        evictionOccurred
            ? insertionLine.blockAddress
            : 0;

    insertionLine.valid = true;
    insertionLine.dirty =
        access.type == AccessType::Write &&
        writePolicy_ == WritePolicy::WriteBack;
    insertionLine.tag = tag;
    insertionLine.blockAddress = blockAddress;
    insertionLine.insertionOrder = accessCounter_;
    insertionLine.lastAccessOrder = accessCounter_;

    return {
        false,
        false,
        evictionOccurred,
        dirtyEviction,
        evictedBlockAddress
    };
}