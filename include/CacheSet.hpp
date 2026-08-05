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

    CacheLine& selectLineForInsertion();

private:
    std::vector<CacheLine> lines_;
    std::size_t nextReplacementIndex_ = 0;
};

#endif