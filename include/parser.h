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

struct JsonValue;
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

struct JsonValue {
    enum Type { INT, STRING, OBJECT, ARRAY } type;
    std::variant<int, std::string, JsonObject, JsonArray> value;

    JsonValue() : type(INT), value(0) {}  // Default constructor
    JsonValue(int v) : type(INT), value(v) {}
    JsonValue(const std::string &v) : type(STRING), value(v) {}
    JsonValue(const JsonObject &v) : type(OBJECT), value(v) {}
    JsonValue(const JsonArray &v) : type(ARRAY), value(v) {}
};

class JsonParser {
public:
    JsonValue parse(const std::string &jsonContent);

private:
    JsonValue parseValue(std::istringstream &ss);
    std::string parseString(std::istringstream &ss);
};

class JsonEvaluator {
public:
    JsonEvaluator(const JsonValue &json);
    JsonValue evaluate(const std::string &expression);

private:
    JsonValue jsonRoot;
    JsonValue evaluatePath(const std::string &path);
};

void printJsonValue(const JsonValue &value);