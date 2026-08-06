#include "TraceParser.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

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

        const std::string cleanedLine =
            trim(removeComment(line));

        if (cleanedLine.empty()) {
            continue;
        }

        try {
            addresses.push_back(
                parseAddress(cleanedLine)
            );
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