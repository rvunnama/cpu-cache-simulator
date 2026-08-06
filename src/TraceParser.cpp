#include "TraceParser.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

namespace {

std::string trim(const std::string& text) {
    const auto first = std::find_if_not(
        text.begin(),
        text.end(),
        [](unsigned char character) {
            return std::isspace(character);
        }
    );

    const auto last = std::find_if_not(
        text.rbegin(),
        text.rend(),
        [](unsigned char character) {
            return std::isspace(character);
        }
    ).base();

    if (first >= last) {
        return "";
    }

    return std::string(first, last);
}

std::string removeComment(const std::string& text) {
    const std::size_t commentPosition = text.find('#');

    if (commentPosition == std::string::npos) {
        return text;
    }

    return text.substr(0, commentPosition);
}

}  // namespace

std::vector<MemoryAccess> TraceParser::parseFile(
    const std::string& filePath
) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error(
            "Unable to open trace file: " + filePath
        );
    }

    std::vector<MemoryAccess> accesses;
    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(inputFile, line)) {
        ++lineNumber;

        const std::string cleanedLine =
            trim(removeComment(line));

        if (cleanedLine.empty()) {
            continue;
        }

        try {
            accesses.push_back(
                parseLine(cleanedLine)
            );
        } catch (const std::exception& error) {
            throw std::runtime_error(
                "Invalid trace entry on line " +
                std::to_string(lineNumber) +
                ": " +
                error.what()
            );
        }
    }

    return accesses;
}

MemoryAccess TraceParser::parseLine(
    const std::string& text
) {
    std::istringstream stream(text);

    std::string firstToken;
    std::string secondToken;

    stream >> firstToken;

    if (!(stream >> secondToken)) {
        // Backward compatibility:
        // an address by itself is treated as a read.
        secondToken = firstToken;
        firstToken = "R";
    }

    std::string extraToken;

    if (stream >> extraToken) {
        throw std::invalid_argument(
            "Unexpected extra data."
        );
    }

    AccessType type;

    if (firstToken == "R" || firstToken == "r") {
        type = AccessType::Read;
    } else if (
        firstToken == "W" ||
        firstToken == "w"
    ) {
        type = AccessType::Write;
    } else {
        throw std::invalid_argument(
            "Access type must be R or W."
        );
    }

    std::size_t processedCharacters = 0;

    const std::uint64_t address = std::stoull(
        secondToken,
        &processedCharacters,
        0
    );

    if (processedCharacters != secondToken.size()) {
        throw std::invalid_argument(
            "Unexpected characters after address."
        );
    }

    return {
        type,
        address
    };
}