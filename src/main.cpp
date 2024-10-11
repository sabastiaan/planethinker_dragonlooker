#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>

#include "parser.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <json_file> <expression>" << std::endl;
        return 1;
    }

    // Read JSON file
    std::ifstream jsonFile(argv[1]);
    if (!jsonFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonContent = buffer.str();

    try {
        // Parse JSON
        JsonParser parser;
        JsonValue json = parser.parse(jsonContent);

        // Evaluate expression
        JsonEvaluator evaluator(json);
        JsonValue result = evaluator.evaluate(argv[2]);

        // Print result
        printJsonValue(result);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
