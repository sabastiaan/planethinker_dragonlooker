#pragma once
#include "parser.h"
#include <functional>
#include <map>
#include <regex>

class ExpressionEvaluator {
public:
    ExpressionEvaluator(JsonStorage &jsonStorage);
    JsonValue evaluate(const std::string &expression);

private:
    JsonStorage &storage;

    // Parsing and evaluation functions
    JsonValue evaluateExpression(const std::string &expression, std::size_t &pos);
    JsonValue parseFunctionCall(const std::string &expression, std::size_t &pos);
    JsonValue parsePathOrLiteral(const std::string &expression, std::size_t &pos);
    JsonValue parsePath(const std::string &expression, std::size_t &pos);

    // Function evaluators
    JsonValue evaluateMinFunction(const std::vector<JsonValue> &args);
    JsonValue evaluateMaxFunction(const std::vector<JsonValue> &args);
    JsonValue evaluateSizeFunction(const std::vector<JsonValue> &args);

    // Utility functions
    void skipWhitespace(const std::string &expression, std::size_t &pos);
    JsonValue getSize(const JsonValue &value);
    bool compareJsonValues(const JsonValue &lhs, const JsonValue &rhs);

    // Map function names to evaluation functions
    using FunctionEvaluator = std::function<JsonValue(const std::vector<JsonValue> &)>;
    std::map<std::string, FunctionEvaluator> functionsMap;
};
