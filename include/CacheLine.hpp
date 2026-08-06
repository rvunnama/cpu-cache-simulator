#ifndef CACHE_LINE_HPP
#define CACHE_LINE_HPP

#include <cstddef>
#include <cstdint>

struct CacheLine {
    bool valid = false;
    bool dirty = false;

    std::uint64_t tag = 0;
    std::uint64_t blockAddress = 0;

    std::size_t insertionOrder = 0;
    std::size_t lastAccessOrder = 0;
};

#endif