#include "TraceParser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::uint64_t> TraceParser::parseFile(
    const std::string& filePath
) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error(
            "Unable to open trace file: " + filePath
        );
    }

    std::vector<std::uint64_t> addresses;
    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(inputFile, line)) {
        ++lineNumber;

        if (line.empty()) {
            continue;
        }

        try {
            addresses.push_back(parseAddress(line));
        } catch (const std::exception& error) {
            throw std::runtime_error(
                "Invalid address on line " +
                std::to_string(lineNumber) +
                ": " +
                error.what()
            );
        }
    }

    return addresses;
}

std::uint64_t TraceParser::parseAddress(
    const std::string& text
) {
    std::size_t processedCharacters = 0;

    const std::uint64_t address = std::stoull(
        text,
        &processedCharacters,
        0
    );

    if (processedCharacters != text.size()) {
        throw std::invalid_argument(
            "Unexpected characters after address."
        );
    }

    return address;
}