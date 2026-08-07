#include "SetAssociativeCache.hpp"
#include "TraceParser.hpp"
#include "CacheVisualizer.hpp"
#include "ReplacementPolicy.hpp"
#include "BenchmarkRunner.hpp"
#include "MemoryAccess.hpp"
#include "WritePolicy.hpp"
#include "WriteMissPolicy.hpp"
#include "CacheAccessResult.hpp"
#include "CacheHierarchy.hpp"
#include "HierarchyAccessResult.hpp"
#include "HierarchyStatistics.hpp"
#include "HierarchyBenchmarkRunner.hpp"
#include "WorkloadGenerator.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <utility>

namespace {

struct ProgramOptions {
    std::size_t cacheSize = 64;
    std::size_t blockSize = 16;
    std::size_t associativity = 1;
    std::string tracePath = "traces/basic.trace";
    std::string csvPath;

    ReplacementPolicy replacementPolicy =
        ReplacementPolicy::FIFO;

    WritePolicy writePolicy =
        WritePolicy::WriteThrough;

    WriteMissPolicy writeMissPolicy =
        WriteMissPolicy::WriteAllocate;

    bool verbose = false;
    bool visualize = false;
    bool step = false;
    bool benchmark = false;

    double cacheAccessTime = 1.0;
    double memoryReadPenalty = 100.0;
    double memoryWritePenalty = 100.0;

    bool hierarchy = false;

    std::size_t l1Size = 32;
    std::size_t l1BlockSize = 16;
    std::size_t l1Associativity = 1;

    std::size_t l2Size = 128;
    std::size_t l2BlockSize = 16;
    std::size_t l2Associativity = 2;

    double l1Latency = 1.0;
    double l2Latency = 10.0;
    double memoryLatency = 100.0;

    bool hierarchyBenchmark = false;

    std::string generatedPattern;

    std::size_t generatedAccessCount = 1000;
    std::uint64_t generatedStartAddress = 0;
    std::uint64_t generatedStride = 16;
    std::size_t generatedWorkingSetSize = 64;

    unsigned int randomSeed = 42;
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
        << " --write-policy <policy>"
        << "  --write-miss-policy <policy>"
        << " [--verbose]\n\n"
        << "[--benchmark]\n\n"
        << "Options:\n"
        << "  --cache-size <bytes>   Total cache capacity\n"
        << "  --block-size <bytes>   Number of bytes per block\n"
        << "  --associativity <ways> Number of lines per set\n"
        << "  --replacement <policy> Replacement policy: fifo or lru\n"
        << "  --trace <file>         Memory trace file\n"
        << "  --verbose              Print every cache access\n"
        << "  --visualize            Print the final cache state\n"
        << "  --step                 Pause after each access\n"
        << "  --benchmark           Compare cache configurations\n"
        << "  --csv <file>          Export benchmark results to CSV\n"
        << "  --write-policy <policy> write-through or write-back\n"
        << "  --write-miss-policy <policy>"
        << " write-allocate or no-write-allocate\n"
        << "  --cache-latency <ns>  Cache access time in nanoseconds\n"
        << "  --memory-read-penalty <ns>"
        << " Main-memory read cost\n"
        << "  --memory-write-penalty <ns>"
        << " Main-memory write cost\n"
        << "  --help                 Show this help message\n\n"
        << "  --hierarchy            Enable two-level L1/L2 mode\n"
        << "  --l1-size <bytes>      L1 cache capacity\n"
        << "  --l1-block-size <bytes> L1 block size\n"
        << "  --l1-associativity <ways> L1 associativity\n"
        << "  --l2-size <bytes>      L2 cache capacity\n"
        << "  --l2-block-size <bytes> L2 block size\n"
        << "  --l2-associativity <ways> L2 associativity\n"
        << "  --l1-latency <ns>      L1 access latency\n"
        << "  --l2-latency <ns>      L2 access latency\n"
        << "  --memory-latency <ns>  Main-memory latency\n"
        << "  --hierarchy-benchmark  Benchmark L1/L2 configurations\n"
        << "  --generate <pattern>   sequential, loop, stride, or random\n"
        << "  --accesses <count>     Number of generated accesses\n"
        << "  --stride <bytes>       Address spacing between generated accesses\n"
        << "  --working-set <count>  Number of addresses in loop/random workload\n"
        << "  --seed <value>         Random workload seed\n"
        << "Example:\n"
        << "  " << programName
        << " --cache-size 64"
        << " --block-size 16"
        << " --associativity 2"
        << " --replacement fifo"
        << " --trace traces/basic.trace"
        << " --verbose\n"
        << " --cache-latency 1"
        << " --memory-penalty 100";
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

    if (text == "lru") {
        return ReplacementPolicy::LRU;
    }

    throw std::invalid_argument(
        "Unsupported replacement policy: " + text +
        ". Supported policies: fifo, lru."
    );
}

WritePolicy parseWritePolicy(
    const std::string& text
) {
    if (text == "write-through") {
        return WritePolicy::WriteThrough;
    }

    if (text == "write-back") {
        return WritePolicy::WriteBack;
    }

    throw std::invalid_argument(
        "Unsupported write policy: " + text +
        ". Supported policies: write-through, write-back."
    );
}

WriteMissPolicy parseWriteMissPolicy(
    const std::string& text
) {
    if (text == "write-allocate") {
        return WriteMissPolicy::WriteAllocate;
    }

    if (text == "no-write-allocate") {
        return WriteMissPolicy::NoWriteAllocate;
    }

    throw std::invalid_argument(
        "Unsupported write-miss policy: " + text +
        ". Supported policies: write-allocate, "
        "no-write-allocate."
    );
}

double parsePositiveDouble(
    const std::string& text,
    const std::string& optionName
) {
    std::size_t processedCharacters = 0;

    const double value = std::stod(
        text,
        &processedCharacters
    );

    if (processedCharacters != text.size()) {
        throw std::invalid_argument(
            "Invalid value for " +
            optionName +
            ": " +
            text
        );
    }

    if (value <= 0.0) {
        throw std::invalid_argument(
            optionName +
            " must be greater than zero."
        );
    }

    return value;
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

std::string writePolicyToString(
    WritePolicy policy
) {
    switch (policy) {
        case WritePolicy::WriteThrough:
            return "Write-through";

        case WritePolicy::WriteBack:
            return "Write-back";
    }

    return "Unknown";
}

std::string writeMissPolicyToString(
    WriteMissPolicy policy
) {
    switch (policy) {
        case WriteMissPolicy::WriteAllocate:
            return "Write-allocate";

        case WriteMissPolicy::NoWriteAllocate:
            return "No-write-allocate";
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

        if (argument == "--benchmark") {
            options.benchmark = true;
            continue;
        }


        if (argument == "--hierarchy") {
            options.hierarchy = true;
            continue;
        }

        if (argument == "--hierarchy-benchmark") {
            options.hierarchyBenchmark = true;
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

        } else if (argument == "--csv") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --csv."
                );
            }

            options.csvPath = argv[++index];

        } else if (argument == "--write-policy") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --write-policy."
                );
            }

            options.writePolicy =
                parseWritePolicy(argv[++index]);

        } else if (argument == "--write-miss-policy") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --write-miss-policy."
                );
            }

            options.writeMissPolicy =
                parseWriteMissPolicy(argv[++index]);

        } else if (argument == "--cache-latency") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --cache-latency."
                );
            }

            options.cacheAccessTime =
                parsePositiveDouble(
                    argv[++index],
                    "--cache-latency"
                );

        } else if (argument == "--memory-read-penalty") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after "
                    "--memory-read-penalty."
                );
            }

            options.memoryReadPenalty =
                parsePositiveDouble(
                    argv[++index],
                    "--memory-read-penalty"
                );

        } else if (argument == "--memory-write-penalty") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after "
                    "--memory-write-penalty."
                );
            }

            options.memoryWritePenalty =
                parsePositiveDouble(
                    argv[++index],
                    "--memory-write-penalty"
                );

        } else if (argument == "--l1-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l1-size."
                );
            }

            options.l1Size =
                parseSize(argv[++index], "--l1-size");

        } else if (argument == "--l1-block-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l1-block-size."
                );
            }

            options.l1BlockSize =
                parseSize(
                    argv[++index],
                    "--l1-block-size"
                );

        } else if (argument == "--l1-associativity") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after "
                    "--l1-associativity."
                );
            }

            options.l1Associativity =
                parseSize(
                    argv[++index],
                    "--l1-associativity"
                );

        } else if (argument == "--l2-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l2-size."
                );
            }

            options.l2Size =
                parseSize(argv[++index], "--l2-size");

        } else if (argument == "--l2-block-size") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l2-block-size."
                );
            }

            options.l2BlockSize =
                parseSize(
                    argv[++index],
                    "--l2-block-size"
                );

        } else if (argument == "--l2-associativity") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after "
                    "--l2-associativity."
                );
            }

            options.l2Associativity =
                parseSize(
                    argv[++index],
                    "--l2-associativity"
                );

        } else if (argument == "--l1-latency") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l1-latency."
                );
            }

            options.l1Latency =
                parsePositiveDouble(
                    argv[++index],
                    "--l1-latency"
                );

        } else if (argument == "--l2-latency") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --l2-latency."
                );
            }

            options.l2Latency =
                parsePositiveDouble(
                    argv[++index],
                    "--l2-latency"
                );

        } else if (argument == "--memory-latency") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --memory-latency."
                );
            }

            options.memoryLatency =
                parsePositiveDouble(
                    argv[++index],
                    "--memory-latency"
                );

        } else if (argument == "--generate") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --generate."
                );
            }

            options.generatedPattern =
                argv[++index];

        } else if (argument == "--accesses") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --accesses."
                );
            }

            options.generatedAccessCount =
                parseSize(
                    argv[++index],
                    "--accesses"
                );

        } else if (argument == "--stride") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --stride."
                );
            }

            options.generatedStride =
                parseSize(
                    argv[++index],
                    "--stride"
                );

        } else if (argument == "--working-set") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --working-set."
                );
            }

            options.generatedWorkingSetSize =
                parseSize(
                    argv[++index],
                    "--working-set"
                );

        } else if (argument == "--seed") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --seed."
                );
            }

            options.randomSeed =
                static_cast<unsigned int>(
                    parseSize(
                        argv[++index],
                        "--seed"
                    )
                );

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
            options.replacementPolicy,
            options.writePolicy,
            options.writeMissPolicy
        );
          
        std::vector<MemoryAccess> accesses;

        if (!options.generatedPattern.empty()) {
            accesses =
                WorkloadGenerator::generate(
                    options.generatedPattern,
                    options.generatedAccessCount,
                    options.generatedStartAddress,
                    options.generatedStride,
                    options.generatedWorkingSetSize,
                    options.randomSeed
                );
        } else {
            accesses =
                TraceParser::parseFile(
                    options.tracePath
                );
        }

        if (options.hierarchyBenchmark) {
            const std::vector<HierarchyBenchmarkResult> results =
                HierarchyBenchmarkRunner::run(
                    accesses,
                    options.l1BlockSize,
                    options.l1Latency,
                    options.l2Latency,
                    options.memoryLatency,
                    options.replacementPolicy,
                    options.writePolicy,
                    options.writeMissPolicy
                );

            HierarchyBenchmarkRunner::printResults(results);

            return 0;
        }
        
        if (options.hierarchy) {
            if (options.l1BlockSize != options.l2BlockSize) {
                throw std::invalid_argument(
                    "L1 and L2 block sizes must match "
                    "in the current hierarchy model."
                );
            }

            SetAssociativeCache l1(
                options.l1Size,
                options.l1BlockSize,
                options.l1Associativity,
                options.replacementPolicy,
                options.writePolicy,
                options.writeMissPolicy
            );

            SetAssociativeCache l2(
                options.l2Size,
                options.l2BlockSize,
                options.l2Associativity,
                options.replacementPolicy,
                options.writePolicy,
                options.writeMissPolicy
            );

            CacheHierarchy hierarchy(
                std::move(l1),
                std::move(l2)
            );

            std::cout << "Two-Level Cache Hierarchy\n";
            std::cout << "L1 size: "
                    << options.l1Size
                    << " bytes\n";
            std::cout << "L1 associativity: "
                    << options.l1Associativity
                    << "-way\n";
            std::cout << "L1 latency: "
                    << options.l1Latency
                    << " ns\n";

            std::cout << "L2 size: "
                    << options.l2Size
                    << " bytes\n";
            std::cout << "L2 associativity: "
                    << options.l2Associativity
                    << "-way\n";
            std::cout << "L2 latency: "
                    << options.l2Latency
                    << " ns\n";

            std::cout << "Memory latency: "
                    << options.memoryLatency
                    << " ns\n\n";

            for (const MemoryAccess& access : accesses) {
                const HierarchyAccessResult result =
                    hierarchy.access(access);

                if (options.verbose) {
                    std::cout
                        << (
                            access.type == AccessType::Read
                                ? "READ "
                                : "WRITE "
                        )
                        << "0x"
                        << std::hex
                        << access.address
                        << std::dec
                        << ": ";

                    if (result.l1Hit) {
                        std::cout << "L1 HIT";
                    } else if (result.l2Hit) {
                        std::cout << "L2 HIT";
                    } else {
                        std::cout << "MEMORY";
                    }

                    std::cout << '\n';
                }
            }

            hierarchy.printStatistics();

            const HierarchyStatistics& statistics =
                hierarchy.getStatistics();

            const double averageHierarchyLatency =
                statistics.calculateAverageAccessTime(
                    options.l1Latency,
                    options.l2Latency,
                    options.memoryLatency
                );

            std::cout << std::fixed
                    << std::setprecision(2);

            std::cout << "Average hierarchy access time: "
                    << averageHierarchyLatency
                    << " ns\n";
        }

        if (options.benchmark) {
            const std::vector<BenchmarkResult> results =
                BenchmarkRunner::run(
                accesses,
                options.cacheAccessTime,
                options.memoryReadPenalty,
                options.memoryWritePenalty
            );

            BenchmarkRunner::printResults(results);

            if (!options.csvPath.empty()) {
                BenchmarkRunner::exportCsv(
                    results,
                    options.csvPath
                );

                std::cout << "\nCSV exported to: "
                        << options.csvPath
                        << '\n';
            }

            return 0;
        }

        std::cout << "CPU Cache Simulator\n";
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
        std::cout << "Write policy: "
                  << writePolicyToString(
                          options.writePolicy
                      )
                  << '\n';
        std::cout << "Write-miss policy: "
                  << writeMissPolicyToString(
                          options.writeMissPolicy
                      )
                  << '\n';
        

        for (const MemoryAccess& access : accesses) {
            const CacheAccessResult result =
                cache.access(access);

            const bool hit = result.hit;

            if (options.verbose) {
                std::cout
                    << "\n"
                    << (
                        access.type == AccessType::Read
                            ? "READ"
                            : "WRITE"
                    )
                    << " 0x"
                    << std::hex
                    << access.address
                    << std::dec
                    << ": "
                    << (hit ? "HIT" : "MISS")
                    << '\n';
            }

            if (options.step) {
                CacheVisualizer::print(cache);
                waitForEnter();
            }
        }
        
        if (options.visualize && !options.step) {
            CacheVisualizer::print(cache);
        }

        cache.printStatistics();

        const CacheStatistics& statistics =
            cache.getStatistics();

        const double traditionalAmat =
            statistics.calculateAmat(
                options.cacheAccessTime,
                options.memoryReadPenalty
            );

        const double averageAccessCost =
            statistics.calculateAverageAccessCost(
                options.cacheAccessTime,
                options.memoryReadPenalty,
                options.memoryWritePenalty
            );

        const double amat =
            statistics.calculateAmat(
                options.cacheAccessTime,
                options.memoryReadPenalty
            );

        std::cout << std::fixed
                  << std::setprecision(2);

        std::cout << "Cache latency: "
                  << options.cacheAccessTime
                  << " ns\n";

        std::cout << "Memory-read penalty: "
                  << options.memoryReadPenalty
                  << " ns\n";

        std::cout << "Memory-write penalty: "
                  << options.memoryWritePenalty
                  << " ns\n";

        std::cout << "Traditional AMAT estimate: "
                  << traditionalAmat
                  << " ns\n";

        std::cout << "Traffic-adjusted access cost: "
                  << averageAccessCost
                  << " ns\n";

    } catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << "\n\n";

        printUsage(argv[0]);

        return 1;
    }

    return 0;
}