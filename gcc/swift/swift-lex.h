#pragma once

#include "swift-buffered-queue.h"
#include "swift-system.h"
#include "swift-token.h"

namespace Swift {

class Lexer {
public:
  Lexer(std::string_view filename, FILE *file)
      : input_source(file), input_queue(input_source), token_source(this),
        token_queue(token_source), filename(filename), file(file), line_map(0),
        current_line(1), current_column(1) {
    line_map = ::linemap_add(::line_table, ::LC_ENTER,
                             /* sysp */ 0, filename.data(),
                             /* current_line */ 1);
  }

  ~Lexer() {
    ::linemap_add(::line_table, ::LC_LEAVE,
                  /* sysp */ 0,
                  /* filename */ NULL,
                  /* to_line */ 0);
  }

  const_TokenPtr peek_token() { return peek_token(0); }
  const_TokenPtr peek_token(int);

  void skip_token() { return skip_token(0); }
  void skip_token(int);

private:
  int peek_input(int n);
  int peek_input() { return peek_input(0); }

  void skip_input(int n);
  void skip_input() { return skip_input(0); }

  location_t get_current_location();

  TokenPtr build_token();

  struct InputSource {
    FILE *input;
    InputSource(FILE *input) : input(input) {}
    int operator()() { return fgetc(input); }
  };
  InputSource input_source;
  buffered_queue<int, InputSource> input_queue;

  struct TokenSource {
    Lexer *lexer;
    TokenSource(Lexer *lexer) : lexer(lexer) {}
    TokenPtr operator()() { return lexer->build_token(); }
  };
  TokenSource token_source;
  buffered_queue<std::shared_ptr<Token>, TokenSource> token_queue;

  std::string_view filename;
  FILE *file;

  const struct line_map *line_map;
  uint32_t current_line;
  uint32_t current_column;
  static const int max_column_hint = 80;
};

} // namespace Swift
