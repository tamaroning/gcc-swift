// { dg-do preprocess { target c++23 } }
// { dg-add-options no_pch }

#include <version>

#ifndef __cpp_lib_expected
# error "Feature-test macro for expected missing in <version>"
#elif __cpp_lib_expected != 202211L
# error "Feature-test macro for expected has wrong value in <version>"
#endif
