#include <priv/brace/tokenizer.h>

#include <unordered_set>

namespace priv::brace {

template<typename T, typename E>
using Result = ::brace::Result<T, E>;
using Unit = ::brace::Unit;

Result<std::vector<Token>, TokenizeError>
Tokenizer::tokenize(const std::string& code) {
    m_line = 1;
    m_column = 1;
    m_current = 0;
    m_tokens.clear();

    TRY(scan_tokens(code));

    add_token(TokenType::Eof);

    return m_tokens;
}

void Tokenizer::skip_whitespace(const std::string& code) {
    while (!is_at_end(code)) {
        char c = peek(code);
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance(code);
        }
        // JSON consumed by this will likely be config
        // So comments are allowed in this JSON library
        else if (c == '/' && peek_next(code) == '/') {  // Single-line comment
            while (peek(code) != '\n' && !is_at_end(code)) {
                advance(code);
            }
        } else if (c == '/' && peek_next(code) == '*') {  // Multiline comment
            advance(code);  // Consume '/'
            advance(code);  // Consume '*'
            while (!is_at_end(code)) {
                if (peek(code) == '*' && peek_next(code) == '/') {
                    advance(code);  // Consume '*'
                    advance(code);  // Consume '/'
                    break;
                }
                advance(code);  // Consume inside comment
            }
        } else {
            break;
        }
    }
}

Result<Unit, TokenizeError> Tokenizer::scan_tokens(const std::string& code) {
    while (!is_at_end(code)) {
        skip_whitespace(code);
        if (is_at_end(code)) {
            break;
        }

        size_t token_start = m_current;
        char c = peek(code);

        if (std::isalpha(c)) {
            auto res = keyword(code);
            if (res.is_err()) {
                return res.unwrap_err();
            }
        } else if (std::isdigit(c)
                   || (c == '-' && std::isdigit(peek_next(code)))) {
            auto res = number(code);
            if (res.is_err()) {
                return res.unwrap_err();
            }
        } else if (c == '\"') {
            auto res = string(code);
            if (res.is_err()) {
                return res.unwrap_err();
            }
        } else if (std::ispunct(c)) {
            auto res = punctuation(code);
            if (res.is_err()) {
                return res.unwrap_err();
            }
        } else {
            return TokenizeError(
                m_line,
                m_column,
                "Unexepcted character: '",
                c,
                "'"
            );
        }
    }
    return Unit {};
}

Result<Unit, TokenizeError> Tokenizer::keyword(const std::string& code) {
    size_t start = m_current;
    while (!is_at_end(code) && std::isalnum(peek(code))) {
        advance(code);
    }

    std::string lexeme = code.substr(start, m_current - start);

    static const std::unordered_set<std::string> keywords =
        {"true", "false", "null"};

    if (keywords.find(lexeme) != keywords.end()) {
        add_token(TokenType::Keyword, lexeme);
    } else {
        return TokenizeError(m_line, m_column, "Unregonized keyword: ", lexeme);
    }

    return Unit {};
}

Result<Unit, TokenizeError> Tokenizer::number(const std::string& code) {
    size_t start = m_current;

    bool has_decimal = false;

    bool is_negative = false;
    if (peek(code) == '-') {
        is_negative = true;
        advance(code);  // Consume '-'
    }

    while (!is_at_end(code) && std::isdigit(peek(code))) {
        advance(code);
    }

    if (!is_at_end(code) && peek(code) == '.') {
        has_decimal = true;
        advance(code);

        if (!std::isdigit(peek(code))) {
            return TokenizeError(m_line, m_column, "Invalid number format");
        }

        while (!is_at_end(code) && std::isdigit(peek(code))) {
            advance(code);
        }
    }

    std::string lexeme = code.substr(start, m_current - start);

    add_token(TokenType::NumberLiteral, lexeme);

    return Unit {};
}

Result<Unit, TokenizeError> Tokenizer::string(const std::string& code) {
    advance(code);
    size_t start = m_current;
    std::string value;

    while (!is_at_end(code) && peek(code) != '\"') {
        if (peek(code) == '\n') {
            return TokenizeError(
                m_line,
                m_column,
                "Unterminated string literal"
            );
        }
        value += advance(code);
    }

    if (is_at_end(code)) {
        return TokenizeError(m_line, m_column, "Unterminated string literal");
    }

    advance(code);
    add_token(TokenType::StringLiteral, value);
    return Unit {};
}

Result<Unit, TokenizeError> Tokenizer::punctuation(const std::string& code) {
    char c = peek(code);
    std::string lexeme(1, c);
    advance(code);

    if (c == ';' || c == ':' || c == ',' || c == '(' || c == ')' || c == '{'
        || c == '}' || c == '[' || c == ']') {
        add_token(TokenType::Punctuation, lexeme);
        return Unit {};
    }

    return TokenizeError(
        m_line,
        m_column,
        "Unrecognized punctuation: ",
        lexeme
    );
}

}  // namespace priv::brace
