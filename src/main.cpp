#include "DirectMappedCache.hpp"
#include "TraceParser.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

namespace {
struct ProgramOptions {
    std::size_t cacheSize = 64;
    std::size_t blockSize = 16;
    std::string tracePath = "traces/basic.trace";
};

void printUsage(const std::string& programName) {
    std::cout
        << "Usage:\n"
        << "  " << programName
        << " --cache-size <bytes>"
        << " --block-size <bytes>"
        << " --trace <file>\n\n"
        << "Example:\n"
        << "  " << programName
        << " --cache-size 64"
        << " --block-size 16"
        << " --trace traces/basic.trace\n";
}

std::size_t parseSize(
    const std::string& text,
    const std::string& optionName
) {
    std::size_t processedCharacters = 0;

    const unsigned long long value = std::stoull(
        text,
        &processedCharacters,
        10
    );

    if (processedCharacters != text.size()) {
        throw std::invalid_argument(
            "Invalid value for " + optionName + ": " + text
        );
    }

    if (value == 0) {
        throw std::invalid_argument(
            optionName + " must be greater than zero."
        );
    }

    return static_cast<std::size_t>(value);
}

ProgramOptions parseArguments(int argc, char* argv[]) {
    ProgramOptions options;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        }

        if (index + 1 >= argc) {
            throw std::invalid_argument(
                "Missing value after option: " + argument
            );
        }

        const std::string value = argv[++index];

        if (argument == "--cache-size") {
            options.cacheSize =
                parseSize(value, "--cache-size");
        } else if (argument == "--block-size") {
            options.blockSize =
                parseSize(value, "--block-size");
        } else if (argument == "--trace") {
            options.tracePath = value;
        } else {
            throw std::invalid_argument(
                "Unknown option: " + argument
            );
        }
    }

    return options;
}
}

int main(int argc, char* argv[]) {
    try {
        const ProgramOptions options =
            parseArguments(argc, argv);

        DirectMappedCache cache(
            options.cacheSize,
            options.blockSize
        );

        const std::vector<std::uint64_t> addresses =
            TraceParser::parseFile(options.tracePath);

        std::cout << "Rhea CPU Cache Simulator\n";
        std::cout << "Cache size: "
                  << options.cacheSize
                  << " bytes\n";
        std::cout << "Block size: "
                  << options.blockSize
                  << " bytes\n";
        std::cout << "Trace file: "
                  << options.tracePath
                  << "\n\n";

        for (const std::uint64_t address : addresses) {
            const bool hit = cache.access(address);

            std::cout << "Address 0x"
                      << std::hex
                      << address
                      << std::dec
                      << ": "
                      << (hit ? "HIT" : "MISS")
                      << '\n';
        }

        cache.printStatistics();
    } catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << "\n\n";

        printUsage(argv[0]);

        return 1;
    }

    return 0;
}