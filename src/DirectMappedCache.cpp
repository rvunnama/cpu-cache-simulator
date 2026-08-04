#include "DirectMappedCache.hpp"

#include <stdexcept>

namespace {
std::size_t validateAndCalculateLineCount(
    std::size_t cacheSize,
    std::size_t blockSize
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

    if (cacheSize % blockSize != 0) {
        throw std::invalid_argument(
            "Cache size must be divisible by block size."
        );
    }

    return cacheSize / blockSize;
}
}

DirectMappedCache::DirectMappedCache(
    std::size_t cacheSize,
    std::size_t blockSize
)
    : cacheSize_(cacheSize),
      blockSize_(blockSize),
      numberOfLines_(
          validateAndCalculateLineCount(cacheSize, blockSize)
      ),
      lines_(numberOfLines_) {
}

bool DirectMappedCache::access(std::uint64_t address) {
    const std::uint64_t blockAddress =
    calculateBlockAddress(address);

    const std::size_t index =
        calculateIndex(blockAddress);

    const std::uint64_t tag =
        calculateTag(blockAddress);

    CacheLine& line = lines_[index];

    if (line.valid && line.tag == tag) {
        statistics_.recordHit();
        return true;
    }

    line.valid = true;
    line.tag = tag;
    statistics_.recordMiss();

    return false;
}

void DirectMappedCache::printStatistics() const {
    statistics_.printReport();
}

std::uint64_t DirectMappedCache::calculateBlockAddress(
    std::uint64_t address
) const {
    return address / blockSize_;
}

std::size_t DirectMappedCache::calculateIndex(
    std::uint64_t blockAddress
) const {
    return static_cast<std::size_t>(
        blockAddress % numberOfLines_
    );
}

std::uint64_t DirectMappedCache::calculateTag(
    std::uint64_t blockAddress
) const {
    return blockAddress / numberOfLines_;
}