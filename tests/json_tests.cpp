#include <gtest/gtest.h>
#include "parser.h"

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
    const JsonObject &obj = std::get<JsonObject>(value.value);
    ASSERT_EQ(obj.at("a").type, JsonValue::INT);
    ASSERT_EQ(std::get<int>(obj.at("a").value), 1);
}

TEST(JsonParserTest, ParseArray) {
    JsonParser parser;
    JsonValue value = parser.parse("[1, 2, 3]");
    ASSERT_EQ(value.type, JsonValue::ARRAY);
    const JsonArray &arr = std::get<JsonArray>(value.value);
    ASSERT_EQ(arr.size(), 3);
    ASSERT_EQ(std::get<int>(arr[0].value), 1);
    ASSERT_EQ(std::get<int>(arr[1].value), 2);
    ASSERT_EQ(std::get<int>(arr[2].value), 3);
}

TEST(JsonEvaluatorTest, EvaluateSimplePath) {
    JsonParser parser;
    JsonValue value = parser.parse("{\"a\": {\"b\": 1}}");
    JsonEvaluator evaluator(value);
    JsonValue result = evaluator.evaluate("a.b");
    ASSERT_EQ(result.type, JsonValue::INT);
    ASSERT_EQ(std::get<int>(result.value), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}