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
            << "R 0x0000\n"
            << "W 0x0040 # inline comment\n"
            << "16\n";
    }

    const std::vector<MemoryAccess> accesses =
        TraceParser::parseFile(path);

    assert(accesses.size() == 3);

    assert(accesses[0].type == AccessType::Read);
    assert(accesses[0].address == 0);

    assert(accesses[1].type == AccessType::Write);
    assert(accesses[1].address == 64);

    // Address-only entries default to reads.
    assert(accesses[2].type == AccessType::Read);
    assert(accesses[2].address == 16);

    std::remove(path.c_str());

    std::cout << "All trace parser tests passed.\n";

    return 0;
}