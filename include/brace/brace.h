#ifndef __BRACE_JSON_H__
#define __BRACE_JSON_H__

#include <cassert>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../priv/brace/tokenizer.h"
#include "result.h"

namespace brace {

class JsonValue;

struct JsonNullValue {};

using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

/**
 * @brief Represents a flexible JSON value that can hold different types of data.
 *
 * Supported JSON value types:
 * - Null (JsonNullValue)
 * - Boolean
 * - Number (double)
 * - String
 * - Object (unordered map of key-value pairs)
 * - Array (vector of JsonValues)
 */
class JsonValue {
  public:
    /**
    * @brief Internal type definition for storing different JSON value types.
    */
    using Value = std::variant<
        bool,
        double,
        std::string,
        JsonObject,
        JsonArray,
        JsonNullValue>;

    /**
     * @brief Default constructor. Creates a null JSON value.
     */
    JsonValue() : m_value(JsonNullValue {}) {}

    /**
     * @brief Constructs a JSON value from a boolean.
     *
     * @param b The boolean value to store
     */
    JsonValue(bool b) : m_value(b) {}

    /**
     * @brief Constructs a JSON value from a numeric value.
     *
     * @param d The numeric value to store (as a double)
     */
    JsonValue(double d) : m_value(d) {}

    /**
     * @brief Constructs a JSON value from a string.
     *
     * @param s The string to store
     */
    JsonValue(const std::string& s) : m_value(s) {}

    /**
     * @brief Constructs a JSON value from an object (map of key-value pairs).
     *
     * @param obj The JSON object to store
     */
    JsonValue(const JsonObject& obj) : m_value(obj) {}

    /**
     * @brief Constructs a JSON value from an array.
     *
     * @param arr The JSON array to store
     */
    JsonValue(const JsonArray& arr) : m_value(arr) {}

    /**
     * @brief Checks if the current value is null.
     *
     * @return true if the value is null, false otherwise
     */
    inline bool is_null() const {
        return std::holds_alternative<JsonNullValue>(m_value);
    }

    /**
     * @brief Checks if the current value is a boolean.
     *
     * @return true if the value is a boolean, false otherwise
     */
    inline bool is_bool() const {
        return std::holds_alternative<bool>(m_value);
    }

    /**
     * @brief Checks if the current value is a number.
     *
     * @return true if the value is a numeric value, false otherwise
     */
    inline bool is_number() const {
        return std::holds_alternative<double>(m_value);
    }

    /**
     * @brief Checks if the current value is a string.
     *
     * @return true if the value is a string, false otherwise
     */
    inline bool is_string() const {
        return std::holds_alternative<std::string>(m_value);
    }

    /**
     * @brief Checks if the current value is an object.
     *
     * @return true if the value is an object, false otherwise
     */
    inline bool is_object() const {
        return std::holds_alternative<JsonObject>(m_value);
    }

    /**
     * @brief Checks if the current value is an array.
     *
     * @return true if the value is an array, false otherwise
     */
    inline bool is_array() const {
        return std::holds_alternative<JsonArray>(m_value);
    }

    /**
     * @brief Retrieves the internal variant value.
     *
     * @return const Reference to the underlying std::variant value
     */
    inline const Value& value() const {
        return m_value;
    }

    /**
     * @brief Converts the JsonValue to a string.
     *
     * @pre The JsonValue must be a string type
     * @throws Asserts in debug build the value must be a string.
     * @return The stored string value
     */
    inline operator std::string() const {
        assert(is_string() && "JsonValue is not a string");
        return std::get<std::string>(m_value);
    }

    /**
     * @brief Converts the JsonValue to an integer.
     *
     * @pre The JsonValue must be a numeric type
     * @throws Asserts in debug build the value must be a number.
     * @return The stored value converted to an int
     */
    inline operator int() const {
        assert(is_number() && "JsonValue is not a number");
        return static_cast<int>(std::get<double>(m_value));
    }

    /**
     * @brief Converts the JsonValue to a double.
     *
     * @pre The JsonValue must be a numeric type
     * @throws Asserts in debug build the value must be a number.
     * @return The stored value converted to a double
     */
    inline operator double() const {
        assert(is_number() && "JsonValue is not a number");
        return std::get<double>(m_value);
    }

    /**
     * @brief Converts the JsonValue to a float.
     *
     * @pre The JsonValue must be a numeric type
     * @throws Asserts in debug build the value must be a number.
     * @return The stored value converted to a float
     */
    inline operator float() const {
        assert(is_number() && "JsonValue is not a number");
        return static_cast<float>(std::get<double>(m_value));
    }

    /**
     * @brief Converts the JsonValue to an size_t.
     *
     * @pre The JsonValue must be a numeric type
     * @throws Asserts in debug build the value must be a number.
     * @return The stored value converted to an size_t
     */
    inline operator size_t() const {
        assert(is_number() && "JsonValue is not a number");
        return static_cast<size_t>(std::get<double>(m_value));
    }

    /**
     * @brief Converts the JsonValue to a bool.
     *
     * @pre The JsonValue must be a boolean type
     * @throws Asserts in debug build the value must be a bool.
     * @return The stored value converted to an bool
     */
    inline operator bool() const {
        assert(is_bool() && "JsonValue is not a bool");
        return std::get<bool>(m_value);
    }

    /**
     * @brief Retrieves the array value of the JsonValue.
     *
     * @pre The JsonValue must be an array type
     * @throws Asserts in debug build the value must be an array.
     * @return Reference to the stored JsonArray
     */
    inline const JsonArray& to_array() const {
        assert(is_array() && "JsonValue is not an array");
        return std::get<JsonArray>(m_value);
    }

    /**
     * @brief Compares the JsonValue with a string.
     *
     * @param other String to compare against
     * @return true if the JsonValue is a string and matches the provided string, false otherwise
     */
    inline bool operator==(const std::string& other) const {
        if (std::holds_alternative<std::string>(m_value)) {
            return std::get<std::string>(m_value) == other;
        }
        return false;
    }

    /**
     * @brief Compares the JsonValue with a C-style string.
     *
     * @param other C-string to compare against
     * @return true if the JsonValue is a string and matches the provided C-string, false otherwise
     */
    inline bool operator==(const char* other) const {
        return *this == std::string(other);
    }

    /**
     * @brief Compares the JsonValue with a double.
     *
     * @param other Double value to compare against
     * @return true if the JsonValue is a number and matches the provided double, false otherwise
     */
    inline bool operator==(double other) const {
        if (std::holds_alternative<double>(m_value)) {
            return std::get<double>(m_value) == other;
        }
        return false;
    }

    /**
     * @brief Compares the JsonValue with an integer.
     *
     * @param other Integer value to compare against
     * @return true if the JsonValue is a number and matches the provided integer, false otherwise
     */
    inline bool operator==(int other) const {
        if (std::holds_alternative<double>(m_value)) {
            return std::get<double>(m_value) == other;
        }
        return false;
    }

    /**
     * @brief Accesses an object's value by string key.
     *
     * @param key The key to look up in the JSON object
     * @pre The JsonValue must be an object type
     * @throws Asserts in debug builds the value must be an object
     * @return Reference to the JsonValue associated with the key
     */
    inline const JsonValue& operator[](const std::string& key) const {
        return operator[](key.c_str());
    }

    inline bool contains(const std::string& key) const {
        if (is_object()) {
            auto object = std::get<JsonObject>(m_value);
            auto it = object.find(key);
            return it != object.end();
        }
        return false;
    }

    /**
     * @brief Accesses an object's value by C-style string key.
     *
     * @param key The C-string key to look up in the JSON object
     * @pre The JsonValue must be an object type
     * @throws Asserts in debug builds the value must be an object
     * @return Reference to the JsonValue associated with the key
     */
    inline const JsonValue& operator[](const char* key) const {
        assert(is_object() && "JsonValue is not an object");
        return std::get<JsonObject>(m_value).at(key);
    }

    /**
     * @brief Accesses an array's value by index.
     *
     * @param index The index to access in the JSON array
     * @pre The JsonValue must be an array type
     * @throws Asserts in debug builds the value must be an array
     * @return Reference to the JsonValue at the specified index
     */
    inline const JsonValue& operator[](size_t index) const {
        assert(is_array() && "JsonValue is not an array");
        return std::get<JsonArray>(m_value).at(index);
    }

  private:
    Value m_value;
};

class ParseError {
  public:
    template<typename... Args>
    ParseError(size_t line, size_t column, Args&&... args) :
        m_line(line),
        m_column(column) {
        std::ostringstream oss;
        (oss << ... << args);
        m_message = oss.str();
    }

    std::string_view message() const {
        return m_message;
    }

    operator std::string() const {
        return m_message;
    }

  private:
    std::string m_message;
    size_t m_line;
    size_t m_column;
};

/**
 * @brief A JSON parsing class for converting JSON strings to JsonValue objects.
 *
 * This class provides functionality to parse JSON-formatted strings into
 * strongly-typed JsonValue objects. It handles various JSON data types
 * and provides error reporting through the ParseError mechanism.
 *
 * The parser supports:
 * - Parsing JSON objects
 * - Parsing JSON arrays
 * - Handling null, boolean, number, and string values
 * - Error detection and reporting
 */
class Parser {
  public:
    /**
     * @brief Parses a JSON-formatted string into a JsonValue.
     *
     * Attempts to convert a JSON-formatted string into a structured JsonValue
     * that can represent various JSON data types.
     *
     * @param json The JSON-formatted string to parse
     * @return A Result object containing either:
     *         - A successfully parsed JsonValue on success
     *         - A ParseError describing the parsing failure on error
     *
     * @note This method is the primary interface for JSON parsing.
     */
    Result<JsonValue, ParseError> parse(const std::string& json);

  private:
    std::vector<::priv::brace::Token> m_tokens;
    size_t m_current;

    const ::priv::brace::Token& peek() const {
        return m_tokens[m_current];
    }

    const ::priv::brace::Token& advance() {
        return m_tokens[m_current++];
    }

    bool is_at_end() const {
        return m_current >= m_tokens.size();
    }

    Result<JsonValue, ParseError> parse_value();
    Result<JsonObject, ParseError> parse_object();
    Result<JsonArray, ParseError> parse_array();
};

}  // namespace brace

#endif
