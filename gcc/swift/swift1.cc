// clang-format off
#include "swift-system.h"
#include "coretypes.h"
#include "target.h"
#include "tree.h"
#include "gimple-expr.h"
#include "diagnostic.h"
#include "opts.h"
#include "fold-const.h"
#include "gimplify.h"
#include "stor-layout.h"
#include "debug.h"
#include "convert.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "selftest.h"
// clang-format on

#include <mpfr.h>

#include "swift-parse.h"

namespace Swift {
static void parse_file(const char *filename) {
  FILE *file = fopen(filename, "r");

  Lexer lexer = Lexer(filename, file);
  Parser parser(lexer);

  std::shared_ptr<TopLevelCodeDecl> ast = parser.parse_program();
  if (ast == nullptr) {
    fatal_error(UNKNOWN_LOCATION, "cannot parse file %s", filename);
  }
  /*
  if (file == NULL) {
    fatal_error(UNKNOWN_LOCATION, "cannot open filename %s: %m", filename);
  }

  Lexer lex(filename, file);

  const_TokenPtr tok = lex.peek_token();
  for (;;) {
    location_t loc = tok->get_locus();
    bool has_text =
        tok->get_id() == IDENTIFIER || tok->get_id() == INTEGER_LITERAL ||
        tok->get_id() == FLOAT_LITERAL || tok->get_id() == STRING_LITERAL;

    fprintf(stderr, "<id=%s%s, %s, line=%d, col=%d>\n",
            tok->token_id_to_str().data(),
            has_text ? (std::string(", text=") + tok->get_str()).c_str() : "",
            LOCATION_FILE(loc), LOCATION_LINE(loc), LOCATION_COLUMN(loc));

    if (tok->get_id() == END_OF_FILE)
      break;

    lex.skip_token();
    tok = lex.peek_token();
  }
  */

  fclose(file);
}

static void parse_files(int num_files, const char **files) {
  for (int i = 0; i < num_files; i++) {
    parse_file(files[i]);
  }
}
} // namespace Swift

/* Language-dependent contents of a type.  */

struct GTY(()) lang_type {
  char dummy;
};

/* Language-dependent contents of a decl.  */

struct GTY(()) lang_decl {
  char dummy;
};

/* Language-dependent contents of an identifier.  This must include a
   tree_identifier.  */

struct GTY(()) lang_identifier {
  struct tree_identifier common;
};

/* The resulting tree type.  */

union GTY((desc("TREE_CODE (&%h.generic) == IDENTIFIER_NODE"),
           chain_next("CODE_CONTAINS_STRUCT (TREE_CODE (&%h.generic), "
                      "TS_COMMON) ? ((union lang_tree_node *) TREE_CHAIN "
                      "(&%h.generic)) : NULL"))) lang_tree_node {
  union tree_node GTY((tag("0"), desc("tree_node_structure (&%h)"))) generic;
  struct lang_identifier GTY((tag("1"))) identifier;
};

/* We don't use language_function.  */

struct GTY(()) language_function {
  int dummy;
};

/* Language hooks.  */

static bool swift_langhook_init(void) {
  /* NOTE: Newer versions of GCC use only:
           build_common_tree_nodes (false);
     See Eugene's comment in the comments section. */
  build_common_tree_nodes(false);

  /* I don't know why this has to be done explicitly.  */
  void_list_node = build_tree_list(NULL_TREE, void_type_node);

  build_common_builtin_nodes();

  return true;
}

static void swift_langhook_parse_file(void) {
  Swift::parse_files(num_in_fnames, in_fnames);
}

static tree swift_langhook_type_for_mode(enum machine_mode mode,
                                         int unsignedp) {
  if (mode == TYPE_MODE(float_type_node))
    return float_type_node;

  if (mode == TYPE_MODE(double_type_node))
    return double_type_node;

  if (mode == TYPE_MODE(intQI_type_node))
    return unsignedp ? unsigned_intQI_type_node : intQI_type_node;
  if (mode == TYPE_MODE(intHI_type_node))
    return unsignedp ? unsigned_intHI_type_node : intHI_type_node;
  if (mode == TYPE_MODE(intSI_type_node))
    return unsignedp ? unsigned_intSI_type_node : intSI_type_node;
  if (mode == TYPE_MODE(intDI_type_node))
    return unsignedp ? unsigned_intDI_type_node : intDI_type_node;
  if (mode == TYPE_MODE(intTI_type_node))
    return unsignedp ? unsigned_intTI_type_node : intTI_type_node;

  if (mode == TYPE_MODE(integer_type_node))
    return unsignedp ? unsigned_type_node : integer_type_node;

  if (mode == TYPE_MODE(long_integer_type_node))
    return unsignedp ? long_unsigned_type_node : long_integer_type_node;

  if (mode == TYPE_MODE(long_long_integer_type_node))
    return unsignedp ? long_long_unsigned_type_node
                     : long_long_integer_type_node;

  if (COMPLEX_MODE_P(mode)) {
    if (mode == TYPE_MODE(complex_float_type_node))
      return complex_float_type_node;
    if (mode == TYPE_MODE(complex_double_type_node))
      return complex_double_type_node;
    if (mode == TYPE_MODE(complex_long_double_type_node))
      return complex_long_double_type_node;
    if (mode == TYPE_MODE(complex_integer_type_node) && !unsignedp)
      return complex_integer_type_node;
  }

  /* gcc_unreachable */
  return NULL;
}

static tree swift_langhook_type_for_size(unsigned int bits ATTRIBUTE_UNUSED,
                                         int unsignedp ATTRIBUTE_UNUSED) {
  gcc_unreachable();
  return NULL;
}

/* Record a builtin function.  We just ignore builtin functions.  */

static tree swift_langhook_builtin_function(tree decl) { return decl; }

static bool swift_langhook_global_bindings_p(void) {
  gcc_unreachable();
  return true;
}

static tree swift_langhook_pushdecl(tree decl ATTRIBUTE_UNUSED) {
  gcc_unreachable();
}

static tree swift_langhook_getdecls(void) { return NULL; }

tree convert(tree type, tree expr) {
  if (type == error_mark_node || expr == error_mark_node ||
      TREE_TYPE(expr) == error_mark_node)
    return error_mark_node;

  if (type == TREE_TYPE(expr))
    return expr;

  if (TYPE_MAIN_VARIANT(type) == TYPE_MAIN_VARIANT(TREE_TYPE(expr)))
    return fold_convert(type, expr);

  switch (TREE_CODE(type)) {
  case VOID_TYPE:
  case BOOLEAN_TYPE:
    return fold_convert(type, expr);
  case INTEGER_TYPE:
    return convert_to_integer(type, expr);
  case POINTER_TYPE:
    return convert_to_pointer(type, expr);
  case REAL_TYPE:
    return convert_to_real(type, expr);
  case COMPLEX_TYPE:
    return convert_to_complex(type, expr);
  default:
    break;
  }

  gcc_unreachable();
}

#undef LANG_HOOKS_NAME
#define LANG_HOOKS_NAME "Swift"

#undef LANG_HOOKS_INIT
#define LANG_HOOKS_INIT swift_langhook_init

#undef LANG_HOOKS_PARSE_FILE
#define LANG_HOOKS_PARSE_FILE swift_langhook_parse_file

#undef LANG_HOOKS_TYPE_FOR_MODE
#define LANG_HOOKS_TYPE_FOR_MODE swift_langhook_type_for_mode

#undef LANG_HOOKS_TYPE_FOR_SIZE
#define LANG_HOOKS_TYPE_FOR_SIZE swift_langhook_type_for_size

#undef LANG_HOOKS_BUILTIN_FUNCTION
#define LANG_HOOKS_BUILTIN_FUNCTION swift_langhook_builtin_function

#undef LANG_HOOKS_GLOBAL_BINDINGS_P
#define LANG_HOOKS_GLOBAL_BINDINGS_P swift_langhook_global_bindings_p

#undef LANG_HOOKS_PUSHDECL
#define LANG_HOOKS_PUSHDECL swift_langhook_pushdecl

#undef LANG_HOOKS_GETDECLS
#define LANG_HOOKS_GETDECLS swift_langhook_getdecls

struct lang_hooks lang_hooks = LANG_HOOKS_INITIALIZER;

#include "gt-swift-swift1.h"
#include "gtype-swift.h"
