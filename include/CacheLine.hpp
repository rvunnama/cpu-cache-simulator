#ifndef CACHE_LINE_HPP
#define CACHE_LINE_HPP

#include <cstdint>

struct CacheLine {
    bool valid = false;
    std::uint64_t tag = 0;
};

#endif