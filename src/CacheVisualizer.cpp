#include "CacheVisualizer.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

void CacheVisualizer::print(
    const SetAssociativeCache& cache
) {
    const std::vector<CacheSet>& sets =
        cache.getSets();

    constexpr int setWidth = 8;
    constexpr int wayWidth = 8;
    constexpr int validWidth = 10;
    constexpr int dirtyWidth = 10;
    constexpr int tagWidth = 14;
    constexpr int blockWidth = 16;
    constexpr int insertedWidth = 14;
    constexpr int accessWidth = 14;

    const int tableWidth =
        setWidth +
        wayWidth +
        validWidth +
        dirtyWidth +
        tagWidth +
        blockWidth +
        insertedWidth +
        accessWidth;

    const std::string separator(
        static_cast<std::size_t>(tableWidth),
        '-'
    );

    std::cout << "\nCache State\n";
    std::cout << separator << '\n';

    std::cout
        << std::left
        << std::setw(setWidth) << "Set"
        << std::setw(wayWidth) << "Way"
        << std::setw(validWidth) << "Valid"
        << std::setw(dirtyWidth) << "Dirty"
        << std::setw(tagWidth) << "Tag"
        << std::setw(blockWidth) << "Block"
        << std::setw(insertedWidth) << "Inserted"
        << std::setw(accessWidth) << "Last Access"
        << '\n';

    std::cout << separator << '\n';

    for (
        std::size_t setIndex = 0;
        setIndex < sets.size();
        ++setIndex
    ) {
        const std::vector<CacheLine>& lines =
            sets[setIndex].getLines();

        for (
            std::size_t wayIndex = 0;
            wayIndex < lines.size();
            ++wayIndex
        ) {
            const CacheLine& line =
                lines[wayIndex];

            std::cout
                << std::left
                << std::setw(setWidth)
                << setIndex
                << std::setw(wayWidth)
                << wayIndex
                << std::setw(validWidth)
                << (line.valid ? "YES" : "NO")
                << std::setw(dirtyWidth)
                << (
                    line.valid && line.dirty
                        ? "YES"
                        : "NO"
                );

            if (line.valid) {
                std::cout
                    << "0x"
                    << std::hex
                    << std::setw(tagWidth - 2)
                    << line.tag
                    << std::dec

                    << "0x"
                    << std::hex
                    << std::setw(blockWidth - 2)
                    << line.blockAddress
                    << std::dec

                    << std::setw(insertedWidth)
                    << line.insertionOrder

                    << std::setw(accessWidth)
                    << line.lastAccessOrder;
            } else {
                std::cout
                    << std::setw(tagWidth) << "-"
                    << std::setw(blockWidth) << "-"
                    << std::setw(insertedWidth) << "-"
                    << std::setw(accessWidth) << "-";
            }

            std::cout << '\n';
        }

        if (setIndex + 1 < sets.size()) {
            std::cout << separator << '\n';
        }
    }

    std::cout << separator << '\n';
}