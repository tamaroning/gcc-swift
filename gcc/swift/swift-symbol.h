#pragma once

#include "swift-system.h"
#include "swift-tree.h"
#include <memory>

namespace Swift {

enum SymbolKind { INVALID, VARIABLE, TYPENAME };

struct Symbol {
public:
  Symbol(const std::string &name, SymbolKind kind)
      : name(name), kind(kind), decl(error_mark_node) {
    gcc_assert(name.size() > 0);
  }

  std::string get_name() const { return name; }

  SymbolKind get_kind() const { return kind; }

  void set_tree_decl(Tree decl_) {
    gcc_assert(decl_.get_tree_code() == VAR_DECL);
    decl = decl_;
  }

  Tree get_tree_decl() const { return decl; }

private:
  std::string name;
  SymbolKind kind;
  Tree decl;
};

typedef std::shared_ptr<Symbol> SymbolPtr;
typedef std::shared_ptr<const Symbol> const_SymbolPtr;

struct SymbolMapping {
public:
  void insert(SymbolPtr s);
  SymbolPtr get(const std::string &str) const;

private:
  typedef std::map<std::string, SymbolPtr> Map;
  Map map;
};

} // namespace Swift
