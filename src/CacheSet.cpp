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