#include "parser.h"
#include <cassert>
#include "parser.h"
#include <cassert>

// Implementation of JsonParser methods

JsonValue JsonParser::parse(const std::string &jsonContent) {
    std::istringstream ss(jsonContent);
    return parseValue(ss);
}

JsonValue JsonParser::parseValue(std::istringstream &ss) {
    ss >> std::ws;
    if (ss.peek() == '"') {
        return JsonValue(parseString(ss));
    } else if (ss.peek() == '{') {
        return JsonValue(parseObject(ss));
    } else if (ss.peek() == '[') {
        return JsonValue(parseArray(ss));
    } else if (isdigit(ss.peek()) || ss.peek() == '-') {
        return JsonValue(parseNumber(ss));
    } else if (ss.peek() == 't' || ss.peek() == 'f' || ss.peek() == 'n') {
        std::string literal;
        ss >> literal;
        if (literal == "true") {
            return JsonValue(1); // Represent true as 1
        } else if (literal == "false") {
            return JsonValue(0); // Represent false as 0
        } else if (literal == "null") {
            return JsonValue(); // Default JsonValue
        } else {
            throw std::runtime_error("Invalid literal");
        }
    } else {
        throw std::runtime_error("Invalid JSON value");
    }
}

std::string JsonParser::parseString(std::istringstream &ss) {
    std::string result;
    char ch;
    if (!ss.get(ch) || ch != '"') {
        throw std::runtime_error("Expected '\"' at the start of string");
    }
    while (ss.get(ch)) {
        if (ch == '\\') {
            // Handle escape sequences
            if (!ss.get(ch)) {
                throw std::runtime_error("Unterminated escape sequence");
            }
            switch (ch) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // Parse 4 hex digits
                    std::string hex;
                    for (int i = 0; i < 4; ++i) {
                        if (!ss.get(ch) || !isxdigit(ch)) {
                            throw std::runtime_error("Invalid unicode escape sequence");
                        }
                        hex += ch;
                    }
                    unsigned int codePoint;
                    std::stringstream hexStream(hex);
                    hexStream >> std::hex >> codePoint;
                    // For simplicity, assume codePoint is in ASCII range
                    result += static_cast<char>(codePoint);
                    break;
                }
                default:
                    throw std::runtime_error(std::string("Invalid escape sequence: \\") + ch);
            }
        } else if (ch == '"') {
            // End of string
            return result;
        } else {
            result += ch;
        }
    }
    throw std::runtime_error("Unterminated string");
}

int JsonParser::parseNumber(std::istringstream &ss) {
    std::string numberStr;
    char ch;
    while (ss.peek() != EOF && (isdigit(ss.peek()) || ss.peek() == '-' || ss.peek() == '+')) {
        ss.get(ch);
        numberStr += ch;
    }
    return std::stoi(numberStr);
}

JsonObject JsonParser::parseObject(std::istringstream &ss) {
    JsonObject object;
    char ch;
    ss.get(ch); // Consume '{'
    ss >> std::ws;
    if (ss.peek() == '}') {
        ss.get(ch); // Consume '}'
        return object; // Empty object
    }
    while (true) {
        ss >> std::ws;
        if (ss.peek() != '"') {
            throw std::runtime_error("Expected string key");
        }
        std::string key = parseString(ss);
        ss >> std::ws;
        if (ss.get(ch) && ch != ':') {
            throw std::runtime_error("Expected ':' after key");
        }
        ss >> std::ws;
        JsonValue value = parseValue(ss);
        object[key] = value;
        ss >> std::ws;
        if (ss.peek() == ',') {
            ss.get(ch); // Consume ','
            continue;
        } else if (ss.peek() == '}') {
            ss.get(ch); // Consume '}'
            break;
        } else {
            throw std::runtime_error("Expected ',' or '}' in object");
        }
    }
    return object;
}

JsonArray JsonParser::parseArray(std::istringstream &ss) {
    JsonArray array;
    char ch;
    ss.get(ch); // Consume '['
    ss >> std::ws;
    if (ss.peek() == ']') {
        ss.get(ch); // Consume ']'
        return array; // Empty array
    }
    while (true) {
        ss >> std::ws;
        JsonValue value = parseValue(ss);
        array.push_back(value);
        ss >> std::ws;
        if (ss.peek() == ',') {
            ss.get(ch); // Consume ','
            continue;
        } else if (ss.peek() == ']') {
            ss.get(ch); // Consume ']'
            break;
        } else {
            throw std::runtime_error("Expected ',' or ']' in array");
        }
    }
    return array;
}

// Implementation of JsonPathEvalator methods

JsonPathEvalator::JsonPathEvalator(const JsonValue &json)
    : jsonRoot(json)
{
}

JsonValue JsonPathEvalator::evaluate(const std::string &expression) {
    return evaluate(expression, jsonRoot);
}

JsonValue JsonPathEvalator::evaluate(const std::string &expression, const JsonValue& context) {
    std::size_t pos = 0;
    std::vector<Path> paths = parse_expression_at(expression, pos);
    const JsonValue* currentValue = &context;

    for (const auto& path : paths) {
        if (path.is_object()) {
            if (currentValue->isObject() && currentValue->contains(path.name)) {
                currentValue = &((*currentValue)[path.name]);
            } else {
                throw std::runtime_error("Invalid object path: " + path.name);
            }
        } else if (path.is_array()) {
            if (currentValue->isArray() && path.array_index < currentValue->size()) {
                currentValue = &((*currentValue)[path.array_index]);
            } else {
                throw std::runtime_error("Invalid array index: " + std::to_string(path.array_index));
            }
        } else {
            throw std::runtime_error("Invalid path type");
        }
    }
    return *currentValue;
}

std::vector<Path> JsonPathEvalator::parse_expression_at(const std::string &expression, std::size_t &pos) {
    std::vector<Path> paths;
    while (pos < expression.length()) {
        if (std::isalpha(expression[pos]) || expression[pos] == '_') {
            // Parse object key
            std::size_t start = pos;
            while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) {
                ++pos;
            }
            std::string key = expression.substr(start, pos - start);
            paths.emplace_back(Path::Object, key);
        } else if (expression[pos] == '[') {
            ++pos; // Consume '['
            // Parse the content inside brackets
            JsonValue indexValue = parse_bracket_expression(expression, pos);
            if (expression[pos] != ']') {
                throw std::runtime_error("Expected ']' in expression");
            }
            ++pos; // Consume ']'
            if (indexValue.type == JsonValue::INT) {
                paths.emplace_back(Path::Array, "", std::get<int>(indexValue.value));
            } else if (indexValue.type == JsonValue::STRING) {
                paths.emplace_back(Path::Object, std::get<std::string>(indexValue.value));
            } else {
                throw std::runtime_error("Invalid index type in array access");
            }
        } else if (expression[pos] == '.') {
            ++pos; // Consume '.'
        } else {
            throw std::runtime_error("Invalid character in expression: " + std::string(1, expression[pos]));
        }
    }
    return paths;
}

JsonValue JsonPathEvalator::parse_bracket_expression(const std::string &expression, std::size_t &pos) {
    if (expression[pos] == '"') {
        // Parse string
        std::string strValue = parseStringInExpression(expression, pos);
        return JsonValue(strValue);
    } else if (std::isdigit(expression[pos]) || expression[pos] == '-') {
        // Parse number
        std::size_t start = pos;
        while (pos < expression.length() && (std::isdigit(expression[pos]) || expression[pos] == '-')) {
            ++pos;
        }
        int intValue = std::stoi(expression.substr(start, pos - start));
        return JsonValue(intValue);
    } else {
        // Parse nested expression
        std::size_t start = pos;
        int brackets = 1;
        while (pos < expression.length() && brackets > 0) {
            if (expression[pos] == '[') {
                brackets++;
            } else if (expression[pos] == ']') {
                brackets--;
                if (brackets == 0) {
                    break; // Stop at the matching closing bracket
                }
            }
            pos++;
        }
        if (brackets != 0) {
            throw std::runtime_error("Mismatched brackets in expression");
        }
        std::string nestedExpression = expression.substr(start, pos - start);
        // Evaluate nested expression with root context
        JsonValue indexValue = evaluate(nestedExpression, jsonRoot);
        return indexValue;
    }
}

std::string JsonPathEvalator::parseStringInExpression(const std::string &expression, std::size_t &pos) {
    std::string result;
    if (expression[pos] != '"') {
        throw std::runtime_error("Expected '\"' at the start of string");
    }
    pos++; // Consume the opening quote
    while (pos < expression.length()) {
        char ch = expression[pos++];
        if (ch == '\\') {
            if (pos >= expression.length()) {
                throw std::runtime_error("Unterminated escape sequence in expression");
            }
            ch = expression[pos++];
            switch (ch) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    if (pos + 4 > expression.length()) {
                        throw std::runtime_error("Incomplete unicode escape sequence");
                    }
                    std::string hex = expression.substr(pos, 4);
                    for (char c : hex) {
                        if (!isxdigit(c)) {
                            throw std::runtime_error("Invalid character in unicode escape sequence");
                        }
                    }
                    unsigned int codePoint;
                    std::stringstream hexStream(hex);
                    hexStream >> std::hex >> codePoint;
                    pos += 4;
                    // Simplified handling: assumes codePoint is in ASCII range
                    result += static_cast<char>(codePoint);
                    break;
                }
                default:
                    throw std::runtime_error(std::string("Invalid escape sequence in expression: \\") + ch);
            }
        } else if (ch == '"') {
            // End of string
            return result;
        } else {
            result += ch;
        }
    }
    throw std::runtime_error("Unterminated string in expression");
}

// Implementation of JsonStorage methods

JsonStorage::JsonStorage(const std::string &jsonFileContent) {
    JsonParser parser;
    json_content = parser.parse(jsonFileContent);
}

JsonValue JsonStorage::get(const std::string& path) {
    JsonPathEvalator evaluator(json_content);
    return evaluator.evaluate(path);
}

// Function to print JsonValue

void printJsonValue(const JsonValue &value) {
    switch (value.type) {
        case JsonValue::INT:
            std::cout << std::get<int>(value.value);
            break;
        case JsonValue::STRING:
            std::cout << '"' << std::get<std::string>(value.value) << '"';
            break;
        case JsonValue::OBJECT: {
            std::cout << "{";
            const JsonObject &obj = std::get<JsonObject>(value.value);
            bool first = true;
            for (const auto& [key, val] : obj) {
                if (!first) {
                    std::cout << ", ";
                }
                std::cout << '"' << key << "\": ";
                printJsonValue(val);
                first = false;
            }
            std::cout << "}";
            break;
        }
        case JsonValue::ARRAY: {
            std::cout << "[";
            const JsonArray &arr = std::get<JsonArray>(value.value);
            for (std::size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                printJsonValue(arr[i]);
            }
            std::cout << "]";
            break;
        }
        default:
            break;
    }
}