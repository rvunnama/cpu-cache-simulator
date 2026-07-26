#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

struct CacheLine {
    bool valid = false;
    std::uint64_t tag = 0;
};

class DirectMappedCache {
public:
    DirectMappedCache(std::size_t cacheSize, std::size_t blockSize)
        : cacheSize_(cacheSize),
          blockSize_(blockSize),
          numberOfLines_(cacheSize / blockSize),
          lines_(numberOfLines_) {
    }

    bool access(std::uint64_t address) {
        const std::uint64_t blockAddress = address / blockSize_;
        const std::size_t index = blockAddress % numberOfLines_;
        const std::uint64_t tag = blockAddress / numberOfLines_;

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

    void printStatistics() const {
        const std::size_t total = hits_ + misses_;

        std::cout << "\nStatistics\n";
        std::cout << "----------\n";
        std::cout << "Total accesses: " << total << '\n';
        std::cout << "Hits: " << hits_ << '\n';
        std::cout << "Misses: " << misses_ << '\n';

        if (total > 0) {
            const double hitRate =
                static_cast<double>(hits_) / static_cast<double>(total) * 100.0;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Hit rate: " << hitRate << "%\n";
        }
    }

private:
    std::size_t cacheSize_;
    std::size_t blockSize_;
    std::size_t numberOfLines_;
    std::vector<CacheLine> lines_;

    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};

int main() {
    constexpr std::size_t cacheSize = 64;
    constexpr std::size_t blockSize = 16;

    DirectMappedCache cache(cacheSize, blockSize);

    const std::vector<std::uint64_t> addresses = {
        0,
        4,
        16,
        0,
        64,
        0
    };

    std::cout << "CPU Cache Simulator\n";
    std::cout << "Cache size: " << cacheSize << " bytes\n";
    std::cout << "Block size: " << blockSize << " bytes\n\n";

    for (const std::uint64_t address : addresses) {
        const bool hit = cache.access(address);

        std::cout << "Address " << address
                  << ": " << (hit ? "HIT" : "MISS")
                  << '\n';
    }

    cache.printStatistics();

    return 0;
}