#include "swift-system.h"
#include <memory>

namespace Swift {
namespace AST {

enum TypeKind {
  INT32,
  INT64,
  UINT32,
  UINT64,
  FLOAT,
  DOUBLE,
  BOOL,
};

class Type {
public:
  Type(location_t locus, TypeKind kind) : kind(kind), locus(locus) {}

  TypeKind get_kind() const { return kind; }

  location_t get_locus() const { return locus; }

private:
  TypeKind kind;
  location_t locus;
};

enum ExprKind {
  IF_EXPR,
  BINARY_OP_EXPR,
  UNARY_OP_EXPR,
  INTEGER_LIT_EXPR,
  FLOAT_LIT_EXPR,
  STRING_LIT_EXPR,
};

class Expr {
public:
  virtual ExprKind get_kind() const = 0;

  location_t get_locus() const { return locus; }

private:
  location_t locus;

protected:
  Expr(location_t locus) : locus(locus) {}
};

class IntegerLitExpr : public Expr {
public:
  IntegerLitExpr(location_t locus, int64_t value) : Expr(locus), value(value) {}

  ExprKind get_kind() const override { return INTEGER_LIT_EXPR; }

  int64_t get_value() const { return value; }

private:
  int64_t value;
};

class FloatLitExpr : public Expr {
public:
  FloatLitExpr(location_t locus, double value) : Expr(locus), value(value) {}

  ExprKind get_kind() const override { return FLOAT_LIT_EXPR; }

  double get_value() const { return value; }

private:
  double value;
};

class StringLitExpr : public Expr {
public:
  StringLitExpr(location_t locus, std::string_view value)
      : Expr(locus), value(value) {}

  ExprKind get_kind() const override { return STRING_LIT_EXPR; }

  std::string_view get_value() const { return value; }

private:
  std::string_view value;
};

class IfExpr : public Expr {
public:
  ExprKind get_kind() const override { return IF_EXPR; }

  // TODO:
};

enum BinOpKind {
  BINOP_ADD,
  BINOP_SUB,
  BINOP_MUL,
  BINOP_DIV,
  BINOP_MOD,
  BINOP_EQ,
  BINOP_NEQ,
  BINOP_LT,
  BINOP_GT,
  BINOP_LEQ,
  BINOP_GEQ,
};

class BinaryOpExpr : public Expr {
public:
  BinaryOpExpr(BinOpKind op, std::shared_ptr<Expr> lhs,
               std::shared_ptr<Expr> rhs)
      : Expr(lhs->get_locus()), op(op), lhs(lhs), rhs(rhs) {}

  ExprKind get_kind() const override { return BINARY_OP_EXPR; }

  BinOpKind get_op() const { return op; }

private:
  BinOpKind op;
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
};

enum StmtKind {
  VAR_DECL_STMT,
  EXPR_STMT,
  WHILE_STMT,
};

class Stmt {
public:
  Stmt(location_t locus) : locus(locus) {}

  virtual StmtKind get_kind() const = 0;

  location_t get_locus() const { return locus; }

private:
  location_t locus;
};

class VarDeclStmt : public Stmt {
public:
  VarDeclStmt(location_t locus, std::string ident, std::shared_ptr<Type> type)
      : Stmt(locus), ident(ident), type(type) {}

  StmtKind get_kind() const override { return VAR_DECL_STMT; }

  std::string get_ident() const { return ident; }

  std::shared_ptr<Type> get_type() const { return type; }

private:
  std::string ident;
  std::shared_ptr<Type> type;
};

class ExprStmt : public Stmt {
public:
  ExprStmt(std::shared_ptr<Expr> expr) : Stmt(expr->get_locus()), expr(expr) {}

  StmtKind get_kind() const override { return EXPR_STMT; }

private:
  std::shared_ptr<Expr> expr;
};

class WhileStmt : public Stmt {
public:
  StmtKind get_kind() const override { return WHILE_STMT; }
};

class BraceStmt {
public:
  BraceStmt(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(stmts) {}

  std::vector<std::shared_ptr<Stmt>> get_stmts() const { return stmts; }

private:
  std::vector<std::shared_ptr<Stmt>> stmts;
};

class TopLevelCodeDecl {
public:
  TopLevelCodeDecl(std::shared_ptr<BraceStmt> body) : body(body) {}

  std::shared_ptr<BraceStmt> get_body() const { return body; }

private:
  std::shared_ptr<BraceStmt> body;
};

} // namespace AST
} // namespace Swift
