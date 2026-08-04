#include "SetAssociativeCache.hpp"

#include <stdexcept>

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
}

SetAssociativeCache::SetAssociativeCache(
    std::size_t cacheSize,
    std::size_t blockSize,
    std::size_t associativity
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
      sets_() {

    sets_.reserve(numberOfSets_);

    for (std::size_t index = 0;
         index < numberOfSets_;
         ++index) {
        sets_.emplace_back(associativity_);
    }
}

bool SetAssociativeCache::access(std::uint64_t address) {
    const std::uint64_t blockAddress =
        calculateBlockAddress(address);

    const std::size_t setIndex =
        calculateSetIndex(blockAddress);

    const std::uint64_t tag =
        calculateTag(blockAddress);

    CacheSet& set = sets_[setIndex];
    std::vector<CacheLine>& lines = set.getLines();

    for (CacheLine& line : lines) {
        if (line.valid && line.tag == tag) {
            statistics_.recordHit();
            return true;
        }
    }

    statistics_.recordMiss();

    for (CacheLine& line : lines) {
        if (!line.valid) {
            line.valid = true;
            line.tag = tag;
            return false;
        }
    }

    lines[0].valid = true;
    lines[0].tag = tag;

    return false;
}