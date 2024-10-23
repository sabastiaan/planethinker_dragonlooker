#include "expression.h"
#include <cctype>
#include <stdexcept>

ExpressionEvaluator::ExpressionEvaluator(JsonStorage &jsonStorage) : storage(jsonStorage) {
    functionsMap["min"] = [this](const std::vector<JsonValue> &args) { return evaluateMinFunction(args); };
    functionsMap["max"] = [this](const std::vector<JsonValue> &args) { return evaluateMaxFunction(args); };
    functionsMap["size"] = [this](const std::vector<JsonValue> &args) { return evaluateSizeFunction(args); };
}

JsonValue ExpressionEvaluator::evaluate(const std::string &expression) {
    std::size_t pos = 0;
    JsonValue result = evaluateExpression(expression, pos);
    skipWhitespace(expression, pos);
    if (pos != expression.length()) {
        throw std::runtime_error("Unexpected characters at end of expression");
    }
    return result;
}

JsonValue ExpressionEvaluator::evaluateExpression(const std::string &expression, std::size_t &pos) {
    skipWhitespace(expression, pos);

    if (pos >= expression.length()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    if (std::isalpha(expression[pos]) || expression[pos] == '_') {
        // Parse identifier (could be function or path)
        std::size_t start = pos;
        while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) {
            pos++;
        }
        std::string identifier = expression.substr(start, pos - start);

        skipWhitespace(expression, pos);

        if (pos < expression.length() && expression[pos] == '(') {
            // Function call
            pos = start; // Reset position to start of function name
            return parseFunctionCall(expression, pos);
        } else {
            // Path
            pos = start; // Reset position to start of identifier
            return parsePathOrLiteral(expression, pos);
        }
    } else if (std::isdigit(expression[pos]) || expression[pos] == '-') {
        // Parse number literal
        return parsePathOrLiteral(expression, pos);
    } else {
        throw std::runtime_error(std::string("Invalid character in expression: ") + expression[pos]);
    }
}

JsonValue ExpressionEvaluator::parseFunctionCall(const std::string &expression, std::size_t &pos) {
    // Parse function name
    std::size_t start = pos;
    while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) {
        pos++;
    }
    std::string functionName = expression.substr(start, pos - start);

    skipWhitespace(expression, pos);

    if (pos >= expression.length() || expression[pos] != '(') {
        throw std::runtime_error("Expected '(' after function name");
    }
    pos++; // Consume '('
    skipWhitespace(expression, pos);

    // Parse arguments
    std::vector<JsonValue> args;
    while (pos < expression.length() && expression[pos] != ')') {
        JsonValue arg = evaluateExpression(expression, pos);
        args.push_back(arg);

        skipWhitespace(expression, pos);

        if (pos < expression.length() && expression[pos] == ',') {
            pos++; // Consume ','
            skipWhitespace(expression, pos);
        } else if (pos < expression.length() && expression[pos] == ')') {
            break;
        } else {
            throw std::runtime_error("Expected ',' or ')' in function call");
        }
    }

    if (pos >= expression.length() || expression[pos] != ')') {
        throw std::runtime_error("Expected ')' after function arguments");
    }
    pos++; // Consume ')'

    // Look up the function and evaluate
    auto it = functionsMap.find(functionName);
    if (it == functionsMap.end()) {
        throw std::runtime_error("Unknown function: " + functionName);
    }

    return it->second(args);
}

JsonValue ExpressionEvaluator::parsePathOrLiteral(const std::string &expression, std::size_t &pos) {
    skipWhitespace(expression, pos);

    if (pos >= expression.length()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    if (std::isdigit(expression[pos]) || expression[pos] == '-') {
        // Parse number literal
        std::size_t start = pos;
        bool hasDigits = false;
        if (expression[pos] == '-') {
            pos++;
        }
        while (pos < expression.length() && std::isdigit(expression[pos])) {
            hasDigits = true;
            pos++;
        }
        if (!hasDigits) {
            throw std::runtime_error("Invalid number literal");
        }
        std::string numberStr = expression.substr(start, pos - start);
        int number = std::stoi(numberStr);
        return JsonValue(number);
    } else {
        // Parse path
        return parsePath(expression, pos);
    }
}

inline bool isIdentifierChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

JsonValue ExpressionEvaluator::parsePath(const std::string &expression, std::size_t &pos) {
    std::string path;
    while (pos < expression.length()) {
        if (isIdentifierChar(expression[pos])) {
            // Collect identifier
            std::size_t start = pos;
            while (pos < expression.length() && isIdentifierChar(expression[pos])) {
                pos++;
            }
            path += expression.substr(start, pos - start);
        } else if (expression[pos] == '.') {
            // Dot separator
            path += expression[pos++];
        } else if (expression[pos] == '[') {
            // Handle brackets
            path += expression[pos++];
            int brackets = 1;
            while (pos < expression.length() && brackets > 0) {
                if (expression[pos] == '[') {
                    brackets++;
                } else if (expression[pos] == ']') {
                    brackets--;
                }
                path += expression[pos++];
            }
            if (brackets != 0) {
                throw std::runtime_error("Mismatched brackets in path");
            }
        } else {
            break;
        }
    }
    return storage.get(path);
}


// Function evaluators

JsonValue ExpressionEvaluator::evaluateMinFunction(const std::vector<JsonValue> &args) {
    if (args.empty()) {
        throw std::runtime_error("min function requires at least one argument");
    }

    auto minIter = std::min_element(args.begin(), args.end(), [this](const JsonValue &lhs, const JsonValue &rhs) {
        return compareJsonValues(lhs, rhs);
    });
    return *minIter;
}

JsonValue ExpressionEvaluator::evaluateMaxFunction(const std::vector<JsonValue> &args) {
    if (args.empty()) {
        throw std::runtime_error("max function requires at least one argument");
    }

    auto maxIter = std::max_element(args.begin(), args.end(), [this](const JsonValue &lhs, const JsonValue &rhs) {
        return compareJsonValues(lhs, rhs);
    });
    return *maxIter;
}

JsonValue ExpressionEvaluator::evaluateSizeFunction(const std::vector<JsonValue> &args) {
    if (args.size() != 1) {
        throw std::runtime_error("size function requires exactly one argument");
    }
    return getSize(args[0]);
}

// Utility functions

JsonValue ExpressionEvaluator::getSize(const JsonValue &value) {
    if (value.type == JsonValue::OBJECT) {
        const JsonObject &obj = std::get<JsonObject>(value.value);
        return JsonValue(static_cast<int>(obj.size()));
    } else if (value.type == JsonValue::ARRAY) {
        const JsonArray &arr = std::get<JsonArray>(value.value);
        return JsonValue(static_cast<int>(arr.size()));
    } else if (value.type == JsonValue::STRING) {
        const std::string &str = std::get<std::string>(value.value);
        return JsonValue(static_cast<int>(str.size()));
    }
    throw std::runtime_error("Size not supported for given type");
}

void ExpressionEvaluator::skipWhitespace(const std::string &expression, std::size_t &pos) {
    while (pos < expression.length() && std::isspace(static_cast<unsigned char>(expression[pos]))) {
        pos++;
    }
}

bool ExpressionEvaluator::compareJsonValues(const JsonValue &lhs, const JsonValue &rhs) {
    // Define type order
    auto typeOrder = [](const JsonValue &value) -> int {
        switch (value.type) {
            case JsonValue::INT: return 1;
            case JsonValue::STRING: return 2;
            case JsonValue::ARRAY: return 3;
            case JsonValue::OBJECT: return 4;
            default: return 5;
        }
    };

    int lhsOrder = typeOrder(lhs);
    int rhsOrder = typeOrder(rhs);

    if (lhsOrder != rhsOrder) {
        return lhsOrder < rhsOrder;
    }

    // If types are the same, compare values
    switch (lhs.type) {
        case JsonValue::INT:
            return std::get<int>(lhs.value) < std::get<int>(rhs.value);
        case JsonValue::STRING:
            return std::get<std::string>(lhs.value) < std::get<std::string>(rhs.value);
        case JsonValue::ARRAY:
            return lhs.size() < rhs.size(); // Compare sizes
        case JsonValue::OBJECT:
            return lhs.size() < rhs.size(); // Compare sizes
        default:
            return false;
    }
}
