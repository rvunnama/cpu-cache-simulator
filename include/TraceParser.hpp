#ifndef TRACE_PARSER_HPP
#define TRACE_PARSER_HPP

#include "MemoryAccess.hpp"

#include <string>
#include <vector>

class TraceParser {
public:
    static std::vector<MemoryAccess> parseFile(
        const std::string& filePath
    );

private:
    static MemoryAccess parseLine(
        const std::string& text
    );
};

#endif