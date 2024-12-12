#ifndef __PRIV_JONNY_TOKENIZER_H__
#define __PRIV_JONNY_TOKENIZER_H__

#include <brace/result.h>

#include <sstream>
#include <string>
#include <vector>

namespace priv::brace {

enum class TokenType {
    Keyword,
    NumberLiteral,
    StringLiteral,
    Punctuation,
    Eof
};

struct Token {
    TokenType type;
    std::string lexeme;
    size_t line;
    size_t column;

    Token(
        TokenType type,
        const std::string& lexeme,
        size_t line,
        size_t column
    ) :
        type(type),
        lexeme(lexeme),
        line(line),
        column(column) {}
};

class TokenizeError {
  public:
    template<typename... Args>
    TokenizeError(size_t line, size_t column, Args&&... args) :
        m_line(line),
        m_column(column) {
        std::ostringstream oss;
        (oss << ... << args);  // Fold expression to concatenate arguments
        m_message = oss.str();
    }

    std::string_view message() const {
        return m_message;
    }

    size_t line() const {
        return m_line;
    }

    size_t column() const {
        return m_line;
    }

    operator std::string() const {
        return m_message;
    }

  private:
    std::string m_message;
    size_t m_line;
    size_t m_column;
};

class Tokenizer {
  public:
    ::brace::Result<std::vector<Token>, TokenizeError>
    tokenize(const std::string& code);

  private:
    size_t m_line {0};
    size_t m_column {0};
    size_t m_current {0};
    std::vector<Token> m_tokens;

    bool is_at_end(const std::string& code) const {
        return m_current >= code.length();
    }

    char peek(const std::string& code) {
        if (is_at_end(code)) {
            return '\0';
        }
        return code[m_current];
    }

    char peek_next(const std::string& code) const {
        if (m_current + 1 >= code.length()) {
            return '\0';
        }
        return code[m_current + 1];
    }

    char advance(const std::string& code) {
        char c = code[m_current];
        m_current++;
        if (c == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        return c;
    }

    inline void add_token(TokenType type, const std::string& lexeme) {
        m_tokens.emplace_back(type, lexeme, m_line, m_column - lexeme.length());
    }

    inline void add_token(TokenType type) {
        add_token(type, "");
    }

    template<typename T, typename E>
    using Result = ::brace::Result<T, E>;
    using Unit = ::brace::Unit;

    void skip_whitespace(const std::string& code);
    Result<Unit, TokenizeError> scan_tokens(const std::string& code);
    Result<Unit, TokenizeError> keyword(const std::string& code);
    Result<Unit, TokenizeError> number(const std::string& code);
    Result<Unit, TokenizeError> string(const std::string& code);
    Result<Unit, TokenizeError> punctuation(const std::string& code);
};

}  // namespace priv::brace

#endif
