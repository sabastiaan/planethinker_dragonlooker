#include "parser.h"


JsonValue JsonParser::parse(const std::string &jsonContent) {
    // Very basic implementation that can parse simple JSON structure.
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

JsonValue JsonEvaluator::evaluatePath(const std::string &path) {
    if (path.empty()) { // if path is empty, return the current value of jsonRoot
        return jsonRoot;
    }

    JsonValue* currentValue = &jsonRoot;
    std::istringstream pathStream(path);
    std::string token;

    while (std::getline(pathStream, token, '.')) { // Split by '.' to traverse objects
        size_t bracketPos = token.find('[');
        if (bracketPos != std::string::npos) {
            // Handle array access if brackets are found
            std::string key = token.substr(0, bracketPos);
            int index = std::stoi(token.substr(bracketPos + 1, token.find(']') - bracketPos - 1));

            if (!currentValue->isObject() || !currentValue->contains(key)) {
                throw std::runtime_error("Invalid path: key not found");
            }

            currentValue = &((*currentValue)[key]);
            if (!currentValue->isArray() || index >= currentValue->size()) {
                throw std::runtime_error("Invalid path: array index out of bounds");
            }

            currentValue = &((*currentValue)[index]);
        } else {
            // Handle object access if no brackets are found
            if (!currentValue->isObject() || !currentValue->contains(token)) {
                throw std::runtime_error("Invalid path: key not found");
            }
            currentValue = &((*currentValue)[token]);
        }
    }

    return *currentValue;
}

JsonValue JsonEvaluator::evaluate(const std::string &expression) {
    // Directly use evaluatePath to evaluate the expression
    return evaluatePath(expression);
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