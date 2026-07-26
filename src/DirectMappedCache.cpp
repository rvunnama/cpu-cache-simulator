#include "DirectMappedCache.hpp"

#include <iomanip>
#include <iostream>
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
    const std::uint64_t blockAddress = address / blockSize_;
    const std::size_t index =
        static_cast<std::size_t>(
            blockAddress % numberOfLines_
        );
    const std::uint64_t tag =
        blockAddress / numberOfLines_;

    CacheLine& line = lines_[index];

    if (line.valid && line.tag == tag) {
        ++hits_;
        return true;
    }

    line.valid = true;
    line.tag = tag;
    ++misses_;

    return false;
}

void DirectMappedCache::printStatistics() const {
    const std::size_t total = hits_ + misses_;

    std::cout << "\nStatistics\n";
    std::cout << "----------\n";
    std::cout << "Total accesses: " << total << '\n';
    std::cout << "Hits: " << hits_ << '\n';
    std::cout << "Misses: " << misses_ << '\n';

    if (total > 0) {
        const double hitRate =
            static_cast<double>(hits_) /
            static_cast<double>(total) *
            100.0;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Hit rate: " << hitRate << "%\n";
    }
}