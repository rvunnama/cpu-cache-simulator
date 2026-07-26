#include "DirectMappedCache.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>

int main() {
    try {
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

        std::cout << "Rhea CPU Cache Simulator\n";
        std::cout << "Cache size: "
                  << cacheSize
                  << " bytes\n";
        std::cout << "Block size: "
                  << blockSize
                  << " bytes\n\n";

        for (const std::uint64_t address : addresses) {
            const bool hit = cache.access(address);

            std::cout << "Address "
                      << address
                      << ": "
                      << (hit ? "HIT" : "MISS")
                      << '\n';
        }

        cache.printStatistics();
    } catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';

        return 1;
    }

    return 0;
}