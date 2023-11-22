#include "swift-token.h"

namespace Swift {

std::string_view get_token_description(TokenId tid) {
  switch (tid) {
#define SWIFT_TOKEN(name, descr)                                               \
  case name:                                                                   \
    return descr;
#define SWIFT_TOKEN_KEYWORD(x, y) SWIFT_TOKEN(x, y)
    SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
  default:
    gcc_unreachable();
  }
}

std::string_view token_id_to_str(TokenId tid) {
  switch (tid) {
#define SWIFT_TOKEN(name, _)                                                   \
  case name:                                                                   \
    return #name;
#define SWIFT_TOKEN_KEYWORD(x, y) SWIFT_TOKEN(x, y)
    SWIFT_TOKEN_LIST
#undef SWIFT_TOKEN_KEYWORD
#undef SWIFT_TOKEN
  default:
    gcc_unreachable();
  }
}

} // namespace Swift