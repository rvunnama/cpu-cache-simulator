#include "CacheVisualizer.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

void CacheVisualizer::print(
    const SetAssociativeCache& cache
) {
    const std::vector<CacheSet>& sets =
        cache.getSets();

    std::cout << "\nCache State\n";
    std::cout
        << "--------------------------------------------------\n";

    for (
        std::size_t setIndex = 0;
        setIndex < sets.size();
        ++setIndex
    ) {
        const CacheSet& set = sets[setIndex];
        const std::vector<CacheLine>& lines =
            set.getLines();

        std::cout << "Set " << setIndex << '\n';

        std::cout
            << std::left
            << std::setw(10) << wayIndex
            << std::setw(10)
            << (line.valid ? "YES" : "NO")
            << std::setw(10)
            << (line.dirty ? "YES" : "NO");

        for (
            std::size_t wayIndex = 0;
            wayIndex < lines.size();
            ++wayIndex
        ) {
            const CacheLine& line = lines[wayIndex];

            std::cout
                << std::left
                << std::setw(10) << wayIndex
                << std::setw(10)
                << (line.valid ? "YES" : "NO");

            if (line.valid) {
                std::cout
                    << "0x"
                    << std::hex
                    << std::setw(12)
                    << line.tag
                    << std::dec
                    << std::setw(18)
                    << line.insertionOrder
                    << line.lastAccessOrder;
            } else {
                std::cout
                    << std::setw(14) << "-"
                    << std::setw(18) << "-"
                    << "-";
            }

            std::cout << '\n';
        }

        std::cout
            << "--------------------------------------------------\n";
    }
}