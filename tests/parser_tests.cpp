#include <gtest/gtest.h>
#include "parser.h"
#include "expression.h"

// JsonParser Tests
TEST(JsonParserTest, ParseInt) {
    JsonParser parser;
    JsonValue value = parser.parse("123");
    ASSERT_EQ(value.type, JsonValue::INT);
    ASSERT_EQ(std::get<int>(value.value), 123);
}

TEST(JsonParserTest, ParseString) {
    JsonParser parser;
    JsonValue value = parser.parse("\"hello\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "hello");
}

TEST(JsonParserTest, ParseObject) {
    JsonParser parser;
    JsonValue value = parser.parse("{\"a\": 1}");
    ASSERT_EQ(value.type, JsonValue::OBJECT);
    const JsonObject& obj = std::get<JsonObject>(value.value);
    ASSERT_EQ(obj.at("a").type, JsonValue::INT);
    ASSERT_EQ(std::get<int>(obj.at("a").value), 1);
}

TEST(JsonParserTest, ParseArray) {
    JsonParser parser;
    JsonValue value = parser.parse("[1, 2, 3]");
    ASSERT_EQ(value.type, JsonValue::ARRAY);
    const JsonArray& arr = std::get<JsonArray>(value.value);
    ASSERT_EQ(arr.size(), 3);
    ASSERT_EQ(std::get<int>(arr[0].value), 1);
    ASSERT_EQ(std::get<int>(arr[1].value), 2);
    ASSERT_EQ(std::get<int>(arr[2].value), 3);
}

// Tests for parsing complex strings
TEST(JsonParserTest, ParseStringWithMultipleBackslashes) {
    JsonParser parser;
    JsonValue value = parser.parse("\"\\\\\\\\\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "\\\\");
}

TEST(JsonParserTest, ParseStringWithNestedQuotes) {
    JsonParser parser;
    JsonValue value = parser.parse("\"He said, \\\"Hello\\\"\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "He said, \"Hello\"");
}

TEST(JsonParserTest, ParseStringWithControlCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Line1\\nLine2\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Line1\nLine2");
}
// Tests for parsing strings with JSON control characters
TEST(JsonParserTest, ParseStringWithComma) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Value1, Value2\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Value1, Value2");
}

TEST(JsonParserTest, ParseStringWithPeriod) {
    JsonParser parser;
    JsonValue value = parser.parse("\"file.name.txt\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "file.name.txt");
}

TEST(JsonParserTest, ParseStringWithBrackets) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Array indices: [0], [1], [2]\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Array indices: [0], [1], [2]");
}

TEST(JsonParserTest, ParseStringWithBraces) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Object keys: {key1}, {key2}\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Object keys: {key1}, {key2}");
}

TEST(JsonParserTest, ParseStringWithMixedControlCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Mix: { [ ] }, commas, and .periods.\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Mix: { [ ] }, commas, and .periods.");
}

TEST(JsonParserTest, ParseStringWithNestedControlCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Nested {braces {within} braces}\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Nested {braces {within} braces}");
}

TEST(JsonParserTest, ParseStringWithEscapedQuotesAndControlCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"He said, \\\"Use [brackets] and {braces}\\\"\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "He said, \"Use [brackets] and {braces}\"");
}

TEST(JsonParserTest, ParseStringWithSpecialSymbols) {
    JsonParser parser;
    JsonValue value = parser.parse("\"Special symbols: !@#$%^&*()_+-=<>?\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "Special symbols: !@#$%^&*()_+-=<>?");
}

TEST(JsonParserTest, ParseStringWithJSONSyntaxCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"JSON syntax: { \\\"key\\\": [1, 2, 3] }\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "JSON syntax: { \"key\": [1, 2, 3] }");
}

TEST(JsonParserTest, ParseStringWithAllControlCharacters) {
    JsonParser parser;
    JsonValue value = parser.parse("\"All controls: { [ ] } , .\"");
    ASSERT_EQ(value.type, JsonValue::STRING);
    ASSERT_EQ(std::get<std::string>(value.value), "All controls: { [ ] } , .");
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
