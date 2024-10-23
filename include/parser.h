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

    bool isObject() const { return type == OBJECT; }
    bool isArray() const { return type == ARRAY; }

    bool contains(const std::string &key) const {
        return isObject() && std::get<JsonObject>(value).count(key) > 0;
    }

    // Non-const versions
    JsonValue& operator[](const std::string &key) {
        return std::get<JsonObject>(value)[key];
    }

    JsonValue& operator[](std::size_t index) {
        return std::get<JsonArray>(value)[index];
    }

    // Const versions
    const JsonValue& operator[](const std::string &key) const {
        return std::get<JsonObject>(value).at(key);
    }

    const JsonValue& operator[](std::size_t index) const {
        return std::get<JsonArray>(value).at(index);
    }

    std::size_t size() const {
        return isArray() ? std::get<JsonArray>(value).size() : 0;
    }
};
class Path
{
public:
    enum Type { Terminal, Object, Array } type;

    Path(Type t, const std::string &n, std::size_t index = 0)
        : type(t), name(n), array_index(index) {}

    // Data members
    const std::string name;           // Name of the path
    const std::size_t array_index;    // Index for array paths (default to 0)

    // Functions to check the type
    bool is_terminal() const { return type == Terminal; }
    bool is_object() const { return type == Object; }
    bool is_array() const { return type == Array; }
};

// This converts strings to json values
class JsonParser {
public:
    JsonValue parse(const std::string &jsonContent);

private:
    JsonValue parseValue(std::istringstream &ss);
    std::string parseString(std::istringstream &ss);
    int parseNumber(std::istringstream &ss);
    JsonObject parseObject(std::istringstream &ss);
    JsonArray parseArray(std::istringstream &ss);
};

class JsonPathEvalator {
public:
    JsonPathEvalator(const JsonValue &json);
    JsonValue evaluate(const std::string &expression);

private:
    JsonValue jsonRoot;

    JsonValue evaluate(const std::string &expression, const JsonValue& context);
    std::vector<Path> parse_expression_at(const std::string &expression, std::size_t &pos);
    JsonValue parse_bracket_expression(const std::string &expression, std::size_t &pos);
    std::string parseStringInExpression(const std::string &expression, std::size_t &pos);
};

// Interface for outside, it provides get which will provide the a path
// so get(a.b[3]) -> JsonValue
// It basically wraps parsed content + path evaluator for the expression parts
class JsonStorage
{
public:
    JsonStorage(const std::string & jsonFileContent);
    JsonValue get(const std::string& path);

private:
    JsonValue json_content;
};

void printJsonValue(const JsonValue &value);