#pragma once

#include "swift-symbol.h"

namespace Swift {

class Scope {
public:
  SymbolMapping &get_current_mapping() {
    gcc_assert(!map_stack.empty());
    return map_stack.back();
  }

  void push_scope();
  void pop_scope();

  Scope();

  SymbolPtr lookup(const std::string &str);

private:
  typedef std::vector<SymbolMapping> MapStack;
  MapStack map_stack;
};

} // namespace Swift