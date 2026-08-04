#ifndef TRACE_PARSER_HPP
#define TRACE_PARSER_HPP

#include <cstdint>
#include <string>
#include <vector>

class TraceParser {
public:
    static std::vector<std::uint64_t> parseFile(
        const std::string& filePath
    );

private:
    static std::uint64_t parseAddress(
        const std::string& text
    );
};

#endif