#ifndef CACHE_SET_HPP
#define CACHE_SET_HPP

#include "CacheLine.hpp"

#include <cstddef>
#include <vector>

class CacheSet {
public:
    explicit CacheSet(std::size_t associativity);

    std::vector<CacheLine>& getLines();

    const std::vector<CacheLine>& getLines() const;

private:
    std::vector<CacheLine> lines_;
};

#endif