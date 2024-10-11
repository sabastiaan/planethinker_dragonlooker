#include "parser.h"

JsonValue JsonParser::parse(const std::string &jsonContent) {
    // Very basic implementation that can parse simple JSON structure.
    // For the purpose of this task, let's handle JSON objects like "{ \"a\": { \"b\": 1 } }"
    // or arrays like "[1, 2, 3]".
    std::istringstream ss(jsonContent);
    return parseValue(ss);
}

JsonValue JsonParser::parseValue(std::istringstream &ss) {
    char ch;
    ss >> ch;

    if (ch == '{') {
        // Parse object
        JsonObject obj;
        std::string key;
        while (ss >> ch) {
            if (ch == '"') {
                key = parseString(ss);
                ss >> ch; // Expecting ':'
                if (ch != ':') {
                    throw std::runtime_error("Expected ':' after key in object");
                }
                obj[key] = parseValue(ss);
                ss >> ch; // Expecting ',' or '}'
                if (ch == '}') {
                    break;
                } else if (ch != ',') {
                    throw std::runtime_error("Expected ',' or '}' in object");
                }
            } else if (ch == '}') {
                break;
            }
        }
        return JsonValue(obj);
    } else if (ch == '[') {
        // Parse array
        JsonArray arr;
        while (ss >> ch) {
            if (ch == ']') {
                break;
            } else {
                ss.putback(ch);
                arr.push_back(parseValue(ss));
                ss >> ch; // Expecting ',' or ']'
                if (ch == ']') {
                    break;
                } else if (ch != ',') {
                    throw std::runtime_error("Expected ',' or ']' in array");
                }
            }
        }
        return JsonValue(arr);
    } else if (ch == '"') {
        // Parse string
        return JsonValue(parseString(ss));
    } else if (std::isdigit(ch) || ch == '-') {
        // Parse number
        ss.putback(ch);
        int value;
        ss >> value;
        return JsonValue(value);
    } else {
        throw std::runtime_error("Unexpected character in JSON");
    }
}

std::string JsonParser::parseString(std::istringstream &ss) {
    std::string result;
    char ch;
    while (ss.get(ch)) {
        if (ch == '"') {
            break;
        } else {
            result += ch;
        }
    }
    return result;
}

JsonEvaluator::JsonEvaluator(const JsonValue &json) : jsonRoot(json) {}

JsonValue JsonEvaluator::evaluate(const std::string &expression) {
    // Split the expression by the "." or "["
    return evaluatePath(expression);
}

JsonValue JsonEvaluator::evaluatePath(const std::string &path) {
    // TODO: parse the path, navigate the jsonRoot accordingly and extract value
    return JsonValue(0);
}

void printJsonValue(const JsonValue &value) {
    switch (value.type) {
        case JsonValue::INT:
            std::cout << std::get<int>(value.value);
            break;
        case JsonValue::STRING:
            std::cout << std::get<std::string>(value.value);
            break;
        case JsonValue::OBJECT: {
            const JsonObject &obj = std::get<JsonObject>(value.value);
            std::cout << "{ ";
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                std::cout << '"' << it->first << "\": ";
                printJsonValue(it->second);
                if (std::next(it) != obj.end()) {
                    std::cout << ", ";
                }
            }
            std::cout << " }";
            break;
        }
        case JsonValue::ARRAY: {
            const JsonArray &arr = std::get<JsonArray>(value.value);
            std::cout << "[ ";
            for (size_t i = 0; i < arr.size(); ++i) {
                printJsonValue(arr[i]);
                if (i != arr.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << " ]";
            break;
        }
    }
}
