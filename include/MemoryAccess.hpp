#ifndef MEMORY_ACCESS_HPP
#define MEMORY_ACCESS_HPP

#include <cstdint>

enum class AccessType {
    Read,
    Write
};

struct MemoryAccess {
    AccessType type;
    std::uint64_t address;
};

#endif