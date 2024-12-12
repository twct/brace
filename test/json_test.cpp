
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <brace/brace.h>
#include <doctest/doctest.h>

using namespace brace;

JsonValue parse_json(const std::string& json_str) {
    Parser parser;
    return parser.parse(json_str).unwrap_ok();
}

TEST_CASE("Simple parsing") {
    std::string json_str = R"(
    {
        "data": {
            "hello": "world"
        }
    }
    )";

    auto value = parse_json(json_str);

    SUBCASE("Is Object") {
        CHECK(value.is_object());
    }

    SUBCASE("Hello is World") {
        CHECK(value["data"]["hello"].is_string());
        std::string hello = value["data"]["hello"];
        CHECK(hello == "world");
    }
}

TEST_CASE("More complex parsing") {
    std::string json_str = R"({
        "name": "Jonny",
        "age": 25,
        "active": true,
        "preferences": ["golf", "programming", "reading"],
        "address": {
            "city": "New York",
            "zipcode": "10001",
            "location": {"lat": 40.7128, "lon": -74.0060}
        },
        "children": [],
        "spouse": null,
        "meta": {"version": 1.2, "timestamp": 1700000000}
    })";

    auto value = parse_json(json_str);

    SUBCASE("Name is Jonny") {
        CHECK(value["name"].is_string());
        CHECK(value["name"] == "Jonny");
    }

    SUBCASE("Age is 25") {
        CHECK(value["age"].is_number());
        int age = value["age"];
        CHECK(age == 25);
    }

    SUBCASE("Active is true") {
        CHECK(value["active"].is_bool());
        bool active = value["active"];
        CHECK(active);
    }

    SUBCASE("Preferences is Array") {
        CHECK(value["preferences"].is_array());
        auto& preferences = value["preferences"].to_array();
        CHECK(preferences.size() == 3);
        CHECK(preferences[0] == "golf");
        CHECK(preferences[1] == "programming");
        CHECK(preferences[2] == "reading");
    }

    SUBCASE("Address is Object with Nested Object") {
        CHECK(value["address"].is_object());
        CHECK(value["address"]["city"] == "New York");
        CHECK(value["address"]["zipcode"] == "10001");
        CHECK(value["address"].contains("city"));

        auto& location = value["address"]["location"];
        CHECK(location.is_object());
        CHECK(location["lat"] == 40.7128);
        CHECK(location["lon"] == -74.0060);
    }

    SUBCASE("Children is Empty Array") {
        CHECK(value["children"].is_array());
        CHECK(value["children"].to_array().empty());
    }

    SUBCASE("Meta is Object with Version and Timestamp") {
        CHECK(value["meta"].is_object());
        CHECK(value["meta"]["version"] == 1.2);
        CHECK(value["meta"]["timestamp"] == 1700000000);
    }
}
