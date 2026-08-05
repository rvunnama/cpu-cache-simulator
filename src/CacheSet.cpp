#include "CacheSet.hpp"

#include <stdexcept>

CacheSet::CacheSet(std::size_t associativity)
    : lines_(associativity) {

    if (associativity == 0) {
        throw std::invalid_argument(
            "Associativity must be greater than zero."
        );
    }
}

std::vector<CacheLine>& CacheSet::getLines() {
    return lines_;
}

const std::vector<CacheLine>& CacheSet::getLines() const {
    return lines_;
}

CacheLine& CacheSet::selectLineForInsertion(
    ReplacementPolicy replacementPolicy
) {
    for (CacheLine& line : lines_) {
        if (!line.valid) {
            return line;
        }
    }

    std::size_t victimIndex = 0;

    for (std::size_t index = 1;
         index < lines_.size();
         ++index) {

        if (replacementPolicy == ReplacementPolicy::FIFO) {
            if (
                lines_[index].insertionOrder <
                lines_[victimIndex].insertionOrder
            ) {
                victimIndex = index;
            }
        } else if (
            replacementPolicy == ReplacementPolicy::LRU
        ) {
            if (
                lines_[index].lastAccessOrder <
                lines_[victimIndex].lastAccessOrder
            ) {
                victimIndex = index;
            }
        }
    }

    return lines_.at(victimIndex);
}