#pragma once

#include "swift-ast.h"
#include "swift-lex.h"
#include "swift-system.h"
#include "swift-token.h"

namespace Swift {

using namespace AST;

class Parser {
public:
  Parser(Lexer &lexer) : lexer(lexer) {}

  std::shared_ptr<TopLevelCodeDecl> parse_program();

  std::shared_ptr<BraceStmt> parse_stmt_list();

  std::shared_ptr<Stmt> parse_stmt();

  std::shared_ptr<VarDeclStmt> parse_var_decl_stmt();

  std::shared_ptr<ExprStmt> parse_expr_stmt();

  std::shared_ptr<WhileStmt> parse_while_stmt();

  std::shared_ptr<Type> parse_type();

  std::shared_ptr<Expr> parse_expr();

  std::shared_ptr<BinaryOpExpr> parse_binary_op_expr(int right_binding_power);

  std::shared_ptr<IfExpr> parse_if_expr();

  std::shared_ptr<BinaryOpExpr> parse_binary_op_expr();

  std::shared_ptr<Expr> parse_literal_expr();

  const_TokenPtr expect_token(TokenId expected) {
    const_TokenPtr actual = lexer.peek_token();
    if (actual->get_id() == expected)
      return actual;
    else {
      error_at(actual->get_locus(), "expected %s, but got %s",
               get_token_description(expected).data(),
               actual->get_token_description().data());
      return nullptr;
    }
  }

  bool skip_expected_token(TokenId expected) {
    const_TokenPtr actual = expect_token(expected);
    bool is_expected = actual != nullptr;
    if (is_expected)
      lexer.skip_token();

    return is_expected;
  }

  void unexpected_token(const_TokenPtr actual) {
    error_at(actual->get_locus(), "unexpected %s",
             actual->get_token_description().data());
  }

private:
  Lexer &lexer;
};

} // namespace Swift
