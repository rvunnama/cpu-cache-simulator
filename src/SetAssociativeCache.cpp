#include "SetAssociativeCache.hpp"

#include <stdexcept>
#include <vector>

namespace {

std::size_t validateAndCalculateLineCount(
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity
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

    return numberOfLines;
}

}  // namespace

SetAssociativeCache::SetAssociativeCache(
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity,
    ReplacementPolicy replacementPolicy
)
    : cacheSize_(cacheSize),
      blockSize_(blockSize),
      associativity_(associativity),
      numberOfLines_(
          validateAndCalculateLineCount(
              cacheSize,
              blockSize,
              associativity
          )
      ),
      numberOfSets_(numberOfLines_ / associativity_),
      replacementPolicy_(replacementPolicy),
      sets_() {

    sets_.reserve(numberOfSets_);

    for (
        std::size_t index = 0;
        index < numberOfSets_;
        ++index
    ) {
        sets_.emplace_back(associativity_);
    }
}

bool SetAssociativeCache::access(
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

            statistics_.recordHit();
            return true;
        }
    }

    statistics_.recordMiss();

    CacheLine& insertionLine =
        set.selectLineForInsertion(
            replacementPolicy_
        );

    insertionLine.valid = true;
    insertionLine.tag = tag;
    insertionLine.insertionOrder = accessCounter_;
    insertionLine.lastAccessOrder = accessCounter_;

    return false;
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