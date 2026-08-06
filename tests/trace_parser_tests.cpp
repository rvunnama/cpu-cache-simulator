#include "TraceParser.hpp"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <string>

int main() {
    const std::string path = "trace_parser_test.trace";

    {
        std::ofstream output(path);

        output
            << "# parser test\n"
            << "\n"
            << "  0x0000  \n"
            << "0x0040 # inline comment\n"
            << "16\n";
    }

    const std::vector<std::uint64_t> addresses =
        TraceParser::parseFile(path);

    assert(addresses.size() == 3);
    assert(addresses[0] == 0);
    assert(addresses[1] == 64);
    assert(addresses[2] == 16);

    std::remove(path.c_str());

    std::cout << "All trace parser tests passed.\n";

    return 0;
}