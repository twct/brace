#include <brace/brace.h>

#include <algorithm>

namespace brace {

using Token = ::priv::brace::Token;
using TokenType = ::priv::brace::TokenType;
using Tokenizer = ::priv::brace::Tokenizer;

Result<JsonValue, ParseError> Parser::parse(const std::string& json) {
    Tokenizer tokenizer;
    auto maybe_tokens = tokenizer.tokenize(json);

    if (maybe_tokens.is_err()) {
        auto err = maybe_tokens.unwrap_err();
        return ParseError(err.line(), err.column(), err.message());
    }

    m_tokens = maybe_tokens.unwrap_ok();
    m_current = 0;
    return parse_value();
}

Result<JsonValue, ParseError> Parser::parse_value() {
    const Token& token = peek();
    std::string lowercase_lexeme = token.lexeme;
    std::transform(
        lowercase_lexeme.begin(),
        lowercase_lexeme.end(),
        lowercase_lexeme.begin(),
        [](unsigned char c) { return std::tolower(c); }
    );

    if (token.type == TokenType::StringLiteral) {
        advance();
        return JsonValue(token.lexeme);
    } else if (token.type == TokenType::NumberLiteral) {
        advance();
        return JsonValue(std::stod(token.lexeme));
    } else if (token.type == TokenType::Punctuation && token.lexeme == "{") {
        auto res = parse_object();
        if (res.is_ok()) {
            return JsonValue(res.unwrap_ok());
        }
        return res.unwrap_err();
    } else if (token.type == TokenType::Punctuation && token.lexeme == "[") {
        auto res = parse_array();
        if (res.is_ok()) {
            return JsonValue(res.unwrap_ok());
        }
        return res.unwrap_err();
    } else if (token.type == TokenType::Keyword) {
        if (lowercase_lexeme == "true" || lowercase_lexeme == "false") {
            advance();
            return JsonValue(lowercase_lexeme == "true");
        } else if (lowercase_lexeme == "null") {
            advance();

            return JsonValue();
        }
    }

    return ParseError(
        token.line,
        token.column,
        "Unexpected token: ",
        token.lexeme
    );
}

Result<JsonObject, ParseError> Parser::parse_object() {
    JsonObject object;
    advance();  // Consume '{'

    while (!is_at_end() && peek().lexeme != "}") {
        const Token& key_token = advance();  // Key
        if (key_token.type != TokenType::StringLiteral) {
            return ParseError(
                key_token.line,
                key_token.column,
                "Expected string key in object"
            );
        }
        std::string key = key_token.lexeme;

        const Token& colon = advance();
        if (colon.lexeme != ":") {
            return ParseError(
                colon.line,
                colon.column,
                "Expected ':' after key in object"
            );
        }

        TRY_ASSIGN(value, parse_value());

        object[key] = value;

        const Token& next = peek();
        if (next.lexeme == ",") {
            advance();  // Consume ','
        } else if (next.lexeme != "}") {
            return ParseError(
                next.line,
                next.column,
                "Expected ',' or '}' in object"
            );
        }
    }

    advance();  // Consume '}'
    return object;
}

Result<JsonArray, ParseError> Parser::parse_array() {
    JsonArray array;
    advance();  // Consume '['

    while (!is_at_end() && peek().lexeme != "]") {
        TRY_ASSIGN_MOVE(value, parse_value());
        array.emplace_back(value);

        const Token& next = peek();
        if (next.lexeme == ",") {
            advance();  // Consume ','
        } else if (next.lexeme != "]") {
            return ParseError(
                next.line,
                next.column,
                "Expected ',' or ']' in array"
            );
        }
    }

    advance();  // Consume ']'
    return array;
}

}  // namespace brace
