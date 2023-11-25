#pragma once

#include "swift-system.h"
#include <tr1/memory>

namespace Swift {

struct Token;
typedef std::shared_ptr<Token> TokenPtr;
typedef std::shared_ptr<const Token> const_TokenPtr;

#define SWIFT_TOKEN_LIST                                                       \
  SWIFT_TOKEN(FIRST_TOKEN, "<first-token-marker>")                             \
  SWIFT_TOKEN(END_OF_FILE, "end of file")                                      \
  SWIFT_TOKEN(ASTERISK, "*")                                                   \
  SWIFT_TOKEN(EQ, "=")                                                         \
  SWIFT_TOKEN(NEQ, "!=")                                                       \
  SWIFT_TOKEN(LEFT_PAREN, "(")                                                 \
  SWIFT_TOKEN(RIGHT_PAREN, ")")                                                \
  SWIFT_TOKEN(LEFT_CURLY, "{")                                                 \
  SWIFT_TOKEN(RIGHT_CURLY, "}")                                                \
  SWIFT_TOKEN(MINUS, "-")                                                      \
  SWIFT_TOKEN(PLUS, "+")                                                       \
  SWIFT_TOKEN(COLON, ":")                                                      \
  SWIFT_TOKEN(SEMICOLON, ";")                                                  \
  SWIFT_TOKEN(SLASH, "/")                                                      \
  SWIFT_TOKEN(PERCENT, "%")                                                    \
  SWIFT_TOKEN(GT, ">")                                                         \
  SWIFT_TOKEN(LT, "<")                                                         \
  SWIFT_TOKEN(GEQ, ">=")                                                       \
  SWIFT_TOKEN(LEQ, "<=")                                                       \
  SWIFT_TOKEN(IDENTIFIER, "identifier")                                        \
  SWIFT_TOKEN(INTEGER_LITERAL, "integer literal")                              \
  SWIFT_TOKEN(FLOAT_LITERAL, "float literal")                                  \
  SWIFT_TOKEN(STRING_LITERAL, "string literal")                                \
                                                                               \
  SWIFT_TOKEN_KEYWORD(LET, "let")                                              \
  SWIFT_TOKEN_KEYWORD(VAR, "var")                                              \
  SWIFT_TOKEN_KEYWORD(IF, "if")                                                \
  SWIFT_TOKEN_KEYWORD(ELSE, "else")                                            \
  SWIFT_TOKEN_KEYWORD(FOR, "for")                                              \
  SWIFT_TOKEN_KEYWORD(IN, "in")                                                \
  SWIFT_TOKEN_KEYWORD(WHILE, "while")                                          \
  SWIFT_TOKEN_KEYWORD(INT32, "Int32")                                          \
  SWIFT_TOKEN_KEYWORD(INT64, "Int64")                                          \
  SWIFT_TOKEN_KEYWORD(UINT32, "Uint32")                                        \
  SWIFT_TOKEN_KEYWORD(UINT64, "Uint64")                                        \
  SWIFT_TOKEN_KEYWORD(FLOAT, "Float")                                          \
  SWIFT_TOKEN_KEYWORD(DOUBLE, "Double")                                        \
  SWIFT_TOKEN_KEYWORD(FALSE_LIT, "false")                                      \
  SWIFT_TOKEN_KEYWORD(TRUE_LIT, "true")                                        \
                                                                               \
  SWIFT_TOKEN(LAST_TOKEN, "<last-token-marker>")

enum TokenId {
#define SWIFT_TOKEN(name, _) name,
#define SWIFT_TOKEN_KEYWORD(x, y) SWIFT_TOKEN(x, y)
  SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
};

std::string_view get_token_description(TokenId tid);

std::string_view token_id_to_str(TokenId tid);

struct Token {
private:
  Token();
  // do not copy and assign to this class
  Token(const Token &);
  Token &operator=(const Token &);

  Token(TokenId tok_id, location_t locus) : id(tok_id), locus(locus), str("") {}
  Token(TokenId tok_id, location_t locus, const std::string &str)
      : id(tok_id), locus(locus), str(std::string(str)) {}

public:
  static TokenPtr make(TokenId token_id, location_t locus) {
    return TokenPtr(new Token(token_id, locus));
  }

  static TokenPtr make_identifier(location_t locus, const std::string &str) {
    return TokenPtr(new Token(IDENTIFIER, locus, str));
  }

  static TokenPtr make_integer_lit(location_t locus, const std::string &str) {
    return TokenPtr(new Token(INTEGER_LITERAL, locus, str));
  }

  static TokenPtr make_float_lit(location_t locus, const std::string &str) {
    return TokenPtr(new Token(FLOAT_LITERAL, locus, str));
  }

  static TokenPtr make_string_lit(location_t locus, const std::string &str) {
    return TokenPtr(new Token(STRING_LITERAL, locus, str));
  }

  TokenId get_id() const { return id; }

  location_t get_locus() const { return locus; }

  const std::string &get_str() const { return str; }

  std::string_view get_token_description() const {
    switch (id) {
#define SWIFT_TOKEN(name, descr)                                               \
  case name:                                                                   \
    return descr;
#define SWIFT_TOKEN_KEYWORD(x, y) SWIFT_TOKEN(x, y)
      SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
    default:
      swift_unreachable();
    }
  }

  std::string_view token_id_to_str() const {
    switch (id) {
#define SWIFT_TOKEN(name, _)                                                   \
  case name:                                                                   \
    return #name;
#define SWIFT_TOKEN_KEYWORD(x, y) SWIFT_TOKEN(x, y)
      SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
    default:
      swift_unreachable();
    }
  }

private:
  TokenId id;
  location_t locus;
  std::string str;
};

} // namespace Swift
