#include "SetAssociativeCache.hpp"
#include "TraceParser.hpp"
#include "CacheVisualizer.hpp"
#include "ReplacementPolicy.hpp"

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
    std::size_t associativity = 1;
    std::string tracePath = "traces/basic.trace";

    ReplacementPolicy replacementPolicy =
        ReplacementPolicy::FIFO;

    bool verbose = false;
    bool visualize = false;
    bool step = false;
};

void printUsage(const std::string& programName) {
    std::cout
        << "Usage:\n"
        << "  " << programName
        << " --cache-size <bytes>"
        << " --block-size <bytes>"
        << " --associativity <ways>"
        << " --replacement <policy>"
        << " --trace <file>"
        << " [--verbose]\n\n"
        << "Options:\n"
        << "  --cache-size <bytes>   Total cache capacity\n"
        << "  --block-size <bytes>   Number of bytes per block\n"
        << "  --associativity <ways> Number of lines per set\n"
        << "  --replacement <policy> Replacement policy: fifo\n"
        << "  --trace <file>         Memory trace file\n"
        << "  --verbose              Print every cache access\n"
        << "  --visualize            Print the final cache state\n"
        << "  --step                 Pause after each access\n"
        << "  --help                 Show this help message\n\n"
        << "Example:\n"
        << "  " << programName
        << " --cache-size 64"
        << " --block-size 16"
        << " --associativity 2"
        << " --replacement fifo"
        << " --trace traces/basic.trace"
        << " --verbose\n";
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

ReplacementPolicy parseReplacementPolicy(
    const std::string& text
) {
    if (text == "fifo") {
        return ReplacementPolicy::FIFO;
    }

    throw std::invalid_argument(
        "Unsupported replacement policy: " + text +
        ". Currently supported: fifo."
    );
}

std::string replacementPolicyToString(
    ReplacementPolicy policy
) {
    switch (policy) {
        case ReplacementPolicy::FIFO:
            return "FIFO";

        case ReplacementPolicy::LRU:
            return "LRU";
    }

    return "Unknown";
}

ProgramOptions parseArguments(int argc, char* argv[]) {
    ProgramOptions options;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        }

        if (argument == "--verbose") {
            options.verbose = true;
            continue;
        }

        if (argument == "--visualize") {
            options.visualize = true;
            continue;
        }

        if (argument == "--step") {
            options.step = true;
            options.verbose = true;
            continue;
        }

        if (argument == "--cache-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --cache-size."
                );
            }

            options.cacheSize = parseSize(
                argv[++index],
                "--cache-size"
            );

        } else if (argument == "--block-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --block-size."
                );
            }

            options.blockSize = parseSize(
                argv[++index],
                "--block-size"
            );

        } else if (argument == "--associativity") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --associativity."
                );
            }

            options.associativity = parseSize(
                argv[++index],
                "--associativity"
            );

        } else if (argument == "--replacement") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --replacement."
                );
            }

            options.replacementPolicy =
                parseReplacementPolicy(argv[++index]);

        } else if (argument == "--trace") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --trace."
                );
            }

            options.tracePath = argv[++index];

        } else {
            throw std::invalid_argument(
                "Unknown option: " + argument
            );
        }
    }

    return options;
}

void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const ProgramOptions options =
            parseArguments(argc, argv);

        SetAssociativeCache cache(
            options.cacheSize,
            options.blockSize,
            options.associativity,
            options.replacementPolicy
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
        std::cout << "Verbose mode: "
                  << (options.verbose ? "enabled" : "disabled")
                  << "\n\n";
        std::cout << "Associativity: "
                  << options.associativity
                  << "-way\n";
        std::cout << "Replacement policy: "
          << replacementPolicyToString(
                 options.replacementPolicy
             )
          << '\n';
        

        for (const std::uint64_t address : addresses) {
            const bool hit = cache.access(address);

            if (options.verbose) {
                std::cout << "\nAddress 0x"
                        << std::hex
                        << address
                        << std::dec
                        << ": "
                        << (hit ? "HIT" : "MISS")
                        << '\n';
            }

            //if (options.step) {
                //CacheVisualizer::print(cache);
                //waitForEnter();
            //}
        }

       // if (options.visualize && !options.step) {
         //   CacheVisualizer::print(cache);
            // }

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