#include "CacheVisualizer.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

void CacheVisualizer::print(
    const DirectMappedCache& cache
) {
    const std::vector<CacheLine>& lines =
        cache.getCacheLines();

    std::cout << "\nCache State\n";
    std::cout << "---------------------------------\n";
    std::cout << std::left
              << std::setw(10) << "Line"
              << std::setw(10) << "Valid"
              << "Tag\n";
    std::cout << "---------------------------------\n";

    for (std::size_t index = 0; index < lines.size(); ++index) {
        const CacheLine& line = lines[index];

        std::cout << std::left
                  << std::setw(10) << index
                  << std::setw(10)
                  << (line.valid ? "YES" : "NO");

        if (line.valid) {
            std::cout << "0x"
                      << std::hex
                      << line.tag
                      << std::dec;
        } else {
            std::cout << "-";
        }

        std::cout << '\n';
    }

    std::cout << "---------------------------------\n";
}