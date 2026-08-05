#ifndef CACHE_LINE_HPP
#define CACHE_LINE_HPP

#include <cstddef>
#include <cstdint>

struct CacheLine {
    bool valid = false;
    std::uint64_t tag = 0;

    std::size_t insertionOrder = 0;
    std::size_t lastAccessOrder = 0;
};

#endif