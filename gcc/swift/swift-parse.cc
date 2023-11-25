#include "swift-parse.h"
#include "line-map.h"
#include "swift-system.h"
#include "swift-token.h"

namespace Swift {

std::shared_ptr<TopLevelCodeDecl> Parser::parse_program() {
  swift_debug("parsing program");
  auto stmts = parse_stmt_list();
  if (stmts == nullptr) {
    return nullptr;
  }

  auto top_level = TopLevelCodeDecl(stmts);
  return std::make_shared<TopLevelCodeDecl>(top_level);
}

std::shared_ptr<BraceStmt> Parser::parse_stmt_list() {
  std::vector<std::shared_ptr<Stmt>> stmts;
  while (lexer.peek_token()->get_id() != END_OF_FILE) {
    auto stmt = parse_stmt();
    if (stmt == nullptr) {
      return nullptr;
    }
    stmts.push_back(stmt);
  }
  auto brace_stmt = BraceStmt(stmts);
  return std::make_shared<BraceStmt>(brace_stmt);
}

std::shared_ptr<Stmt> Parser::parse_stmt() {
  swift_debug("parsing statement");
  const_TokenPtr t = lexer.peek_token();
  switch (t->get_id()) {
  case VAR:
  case LET:
    return parse_var_decl_stmt();
  case IDENTIFIER:
  case IF:
  case INTEGER_LITERAL:
  case FLOAT_LITERAL:
  case STRING_LITERAL:
  case LEFT_PAREN:
  case RIGHT_PAREN:
  case PLUS:
  case MINUS:
    return parse_expr_stmt();
  case WHILE:
    return parse_while_stmt();
  default:
    unexpected_token(t);
    return nullptr;
  }
}

// let a: Int64;
// var b: Int32;
std::shared_ptr<VarDeclStmt> Parser::parse_var_decl_stmt() {
  const_TokenPtr t = lexer.peek_token();
  location_t locus = t->get_locus();
  switch (t->get_id()) {
  case VAR:
  case LET:
    lexer.skip_token();
    break;
  default:
    unexpected_token(t);
    return nullptr;
  }

  const_TokenPtr ident = expect_token(IDENTIFIER);
  if (ident == nullptr) {
    return nullptr;
  }

  if (!skip_expected_token(COLON)) {
    return nullptr;
  }

  auto type = parse_type();
  if (type == nullptr) {
    return nullptr;
  }
  if (!skip_expected_token(SEMICOLON))
    return nullptr;

  auto decl = VarDeclStmt(locus, ident->get_str(), type);
  return std::make_shared<VarDeclStmt>(decl);
}

std::shared_ptr<ExprStmt> Parser::parse_expr_stmt() {
  auto expr = parse_expr();
  if (expr == nullptr) {
    return nullptr;
  }
  if (!skip_expected_token(SEMICOLON))
    return nullptr;

  return std::make_shared<ExprStmt>(ExprStmt(expr));
}

std::shared_ptr<WhileStmt> Parser::parse_while_stmt() {
  // TODO: implement
  swift_unreachable();
}

std::shared_ptr<Type> Parser::parse_type() {
  const_TokenPtr t = lexer.peek_token();
  // TODO: other types
  switch (t->get_id()) {
  case TokenId::INT32: {
    lexer.skip_token();
    auto ty = Type(t->get_locus(), TypeKind::INT32);
    return std::make_shared<Type>(ty);
  }
  default:
    unexpected_token(t);
    swift_unreachable();
  }
}

// https://github.com/rofirrim/gcc-tiny/blob/4640908fe4c93fe3f45817fc7bc49a28a218664e/gcc/tiny/tiny-parser.cc#L1389
std::shared_ptr<Expr> Parser::parse_expr() {
  swift_debug("parsing expression");
  // TODO: switch
  return parse_literal_expr();
}

std::shared_ptr<IfExpr> Parser::parse_if_expr() {
  // TODO: implement
  swift_unreachable();
}

std::shared_ptr<BinaryOpExpr> Parser::parse_binary_op_expr() {
  // TODO: implement
  swift_unreachable();
}

std::shared_ptr<Expr> Parser::parse_literal_expr() {
  const_TokenPtr t = lexer.peek_token();
  switch (t->get_id()) {
  case INTEGER_LITERAL: {
    lexer.skip_token();
    auto lit = IntegerLitExpr(t->get_locus(), atoi(t->get_str().c_str()));
    return std::make_shared<IntegerLitExpr>(lit);
  }
  case FLOAT_LITERAL: {
    lexer.skip_token();
    auto lit = FloatLitExpr(t->get_locus(), atof(t->get_str().c_str()));
    return std::make_shared<FloatLitExpr>(lit);
  }
  case STRING_LITERAL: {
    lexer.skip_token();
    auto lit = StringLitExpr(t->get_locus(), t->get_str());
    return std::make_shared<StringLitExpr>(lit);
  }
  default:
    swift_unreachable();
  }
}

} // namespace Swift
