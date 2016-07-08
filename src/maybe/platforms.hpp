#pragma once

/**
 * Figure out waht features to enable based on compiler versions.
 *
 * This logic is re-used from std::experimental::optional.
 */

# if defined __GNUC__ // NOTE: GNUC is also defined for Clang
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)
#     define MAYBE_RESULT_GCC_4_8_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define MAYBE_RESULT_GCC_4_8_AND_HIGHER___
#   endif
#
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#     define MAYBE_RESULT_GCC_4_7_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define MAYBE_RESULT_GCC_4_7_AND_HIGHER___
#   endif
#
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ == 8) && (__GNUC_PATCHLEVEL__ >= 1)
#     define MAYBE_RESULT_GCC_4_8_1_AND_HIGHER___
#   elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 9)
#     define MAYBE_RESULT_GCC_4_8_1_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define MAYBE_RESULT_GCC_4_8_1_AND_HIGHER___
#   endif
# endif
#
# if defined __clang_major__
#   if (__clang_major__ == 3 && __clang_minor__ >= 5)
#     define MAYBE_RESULT_CLANG_3_5_AND_HIGHTER_
#   elif (__clang_major__ > 3)
#     define MAYBE_RESULT_CLANG_3_5_AND_HIGHTER_
#   endif
#   if defined MAYBE_RESULT_CLANG_3_5_AND_HIGHTER_
#     define MAYBE_RESULT_CLANG_3_4_2_AND_HIGHER_
#   elif (__clang_major__ == 3 && __clang_minor__ == 4 && __clang_patchlevel__ >= 2)
#     define MAYBE_RESULT_CLANG_3_4_2_AND_HIGHER_
#   endif
# endif

# if defined __clang__
#   if (__clang_major__ > 2) || (__clang_major__ == 2) && (__clang_minor__ >= 9)
#     define MAYBE_RESULT_HAS_THIS_RVALUE_REFS 1
#   else
#     define MAYBE_RESULT_HAS_THIS_RVALUE_REFS 0
#   endif
# elif defined MAYBE_RESULT_GCC_4_8_1_AND_HIGHER___
#   define MAYBE_RESULT_HAS_THIS_RVALUE_REFS 1
# else
#   define MAYBE_RESULT_HAS_THIS_RVALUE_REFS 0
# endif


# if defined MAYBE_RESULT_CLANG_3_5_AND_HIGHTER_ && (defined __cplusplus) && (__cplusplus != 201103L)
#   define MAYBE_RESULT_HAS_MOVE_ACCESSORS 1
# else
#   define MAYBE_RESULT_HAS_MOVE_ACCESSORS 0
# endif

# // In C++11 constexpr implies const, so we need to make non-const members also non-constexpr
# if (defined __cplusplus) && (__cplusplus == 201103L)
#   define MAYBE_RESULT_MUTABLE_CONSTEXPR
# else
#   define MAYBE_RESULT_MUTABLE_CONSTEXPR constexpr
# endif