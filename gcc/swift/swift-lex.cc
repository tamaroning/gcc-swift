#include "swift-lex.h"
#include "input.h"
#include "swift-system.h"

namespace Swift {

int Lexer::peek_input(int n) { return input_queue.peek(n); }

void Lexer::skip_input(int n) { input_queue.skip(n); }

location_t Lexer::get_current_location() {
  return ::linemap_position_for_column(::line_table, current_column);
}

const_TokenPtr Lexer::peek_token(int n) { return token_queue.peek(n); }

void Lexer::skip_token(int n) { token_queue.skip(n); }

namespace {

const std::string keywords[] = {
#define SWIFT_TOKEN(x, y)
#define SWIFT_TOKEN_KEYWORD(name, keyword) keyword,
    SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
};

TokenId keyword_keys[] = {
#define SWIFT_TOKEN(x, y)
#define SWIFT_TOKEN_KEYWORD(name, keyword) name,
    SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
};

const int num_keywords = sizeof(keywords) / sizeof(*keywords);

} // namespace

TokenId classify_keyword(const std::string &str) {
  int idx = 0;
  bool is_keyword = false;
  for (; idx < num_keywords; idx++) {
    if (str == keywords[idx]) {
      is_keyword = true;
      break;
    }
  }

  if (is_keyword)
    return keyword_keys[idx];
  else
    return IDENTIFIER;
}

TokenPtr Lexer::build_token() {
  // symbols
  for (;;) {
    location_t loc = get_current_location();
    int current_char = peek_input();
    skip_input();

    if (current_char == EOF) {
      return Token::make(END_OF_FILE, loc);
    }
    switch (current_char) {
    // whitespaces
    case '\n':
      current_line++;
      current_column = 1;
      linemap_line_start(::line_table, current_line, max_column_hint);
      continue;
    case ' ':
      current_column++;
      continue;
    case '\t':
      // Width of a tab is not well defined, let's assume 8 for now
      current_column += 8;
      continue;
    case '=':
      current_column++;
      return Token::make(EQ, loc);
    case '(':
      current_column++;
      return Token::make(LEFT_PAREN, loc);
    case ')':
      current_column++;
      return Token::make(RIGHT_PAREN, loc);
    case '{':
      current_column++;
      return Token::make(LEFT_CURLY, loc);
    case '}':
      current_column++;
      return Token::make(RIGHT_CURLY, loc);
    case '-':
      current_column++;
      return Token::make(MINUS, loc);
    case '+':
      current_column++;
      return Token::make(PLUS, loc);
    case ';':
      current_column++;
      return Token::make(SEMICOLON, loc);
    case ':':
      current_column++;
      return Token::make(COLON, loc);
    case '<':
      if (peek_input() == '=') {
        skip_input();
        current_column += 2;

        return Token::make(LEQ, loc);
      } else {
        current_column++;
        return Token::make(LT, loc);
      }
      break;
    case '>':
      if (peek_input() == '=') {
        skip_input();
        current_column += 2;

        return Token::make(GEQ, loc);
      } else {
        current_column++;
        return Token::make(GT, loc);
      }
      break;
    case '#': /* comment */
              // FIXME: swift style comments
      current_column++;
      current_char = peek_input();
      while (current_char != '\n') {
        skip_input();
        current_column++; // won't be used
        current_char = peek_input();
      }
      continue;
      break;
    }

    // identifiers or keywords
    if (ISALPHA(current_char) || current_char == '_') {
      std::string str;
      str.reserve(16); // some sensible default
      str += current_char;

      int length = 1;
      current_char = peek_input();
      while (ISALPHA(current_char) || ISDIGIT(current_char) ||
             current_char == '_') {
        length++;

        str += current_char;
        skip_input();
        current_char = peek_input();
      }

      current_column += length;

      TokenId keyword = classify_keyword(str);
      if (keyword == IDENTIFIER) {
        return Token::make_identifier(loc, str);
      } else {
        return Token::make(keyword, loc);
      }
    }

    // integer or flaot literals
    if (ISDIGIT(current_char) || current_char == '.') {
      std::string str;
      str.reserve(16); // some sensible default
      str += current_char;

      bool is_float = (current_char == '.');

      int length = 1;
      current_char = peek_input();
      while (ISDIGIT(current_char) || (!is_float && current_char == '.')) {
        length++;

        is_float = is_float || (current_char == '.');

        str += current_char;
        skip_input();
        current_char = peek_input();
      }

      current_column += length;

      if (is_float)
        return Token::make_float_lit(loc, str);
      else
        return Token::make_integer_lit(loc, str);
    }

    // string literals
    if (current_char == '"') {
      std::string str;
      str.reserve(16); // some sensible default

      int length = 1;
      current_char = peek_input();
      while (current_char != '\n' && current_char != '"') {
        length++;

        str += current_char;
        skip_input();
        current_char = peek_input();
      }

      current_column += length;

      if (current_char == '\n') {
        error_at(get_current_location(), "unended string literal");
      } else if (current_char == '"') {
        skip_input();
      } else {
        gcc_unreachable();
      }

      return Token::make_string_lit(loc, str);
    }

    // Martians
    error_at(loc, "unexpected character '%x'", current_char);
    current_column++;
  }
}

} // namespace Swift
