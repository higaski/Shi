/// Shi
///
/// \file   shi.hpp
/// \author	Vincent Hamp
/// \date   27/07/2016

/// \page page_dictionary Dictionary
///
/// \dot
/// digraph H {
///
///   aHtmlTable [
///     shape=plaintext
///     color=blue      // The color of the border of the table
///     label=<
///
///     <table border='1' cellborder='0'>
///       <tr><td>col 1</td><td>foo</td></tr>
///       <tr><td>COL 2</td><td>bar</td></tr>
///     </table>>
///
///   ];
///
/// }
/// \enddot
///
/// \dot
/// digraph G {
///
///   word[shape=record, label="{link|flags|name|code}"];
///
/// }
/// \enddot
///
/// \dot
/// digraph G {
///
///   rankdir=LR;
///
///	  subgraph cluster_data {
///     label = "data";
///	    node[style=filled];
///     d0[shape=box];
///     d1[style=invis];
///     d2[shape=box];
///     d0->d1->d2[style=invis];
///   }
///
///	  subgraph cluster_text {
///     label = "text";
///	    node[style=filled];
///     t0[style=invis];
///     t1[shape=box];
///     t2[style=invis];
///     t0->t1->t2[style=invis];
///   }
///
///   d0->t1->d2;
///
///   link->d0;
/// }
/// \enddot
///
/// \page page_api API
///
/// http://forth-standard.org/
///
///
///
/// \page page_implementation Implementation
/// sd
///
/// \section section_stack_notation Stack notation
/// blabla
///
/// \page page_lerp Lerp
/// blabla
///
/// ```
/// ( x x1 x2 y1 y2 -- )
/// ```
///
/// LERP ->
/// 'y1 y2 y1 - x x1 - * x2 x1 - / +'
/// 'x x1 x2 y1 y2'
/// over
/// 'x x1 x2 y1 y2 y1'
/// -
/// 'x x1 x2 y1 (y2-y1)'
/// 4 roll
/// 'x1 x2 y1 (y2-y1) x'
/// 4 pick
/// 'x1 x2 y1 (y2-y1) x x1'
/// -
/// 'x1 x2 y1 (y2-y1) (x-x1)'
/// *
/// 'x1 x2 y1 (y2-y1)*(x-x1)'
/// 2swap
/// 'y1 (y2-y1)*(x-x1) x1 x2'
/// swap
/// 'y1 (y2-y1)*(x-x1) x2 x1'
/// -
/// 'y1 (y2-y1)*(x-x1) (x2-x1)'
/// /
/// 'y1 (y2-y1)*(x-x1)/(x2-x1)'
/// +
///
/// ... done
/// : lerp over - 4 roll 4 pick - * 2swap swap - / + ;

#pragma once

/// Data-stack size
#define SHI_STACK_SIZE 64

/// Most flash types are 0xFF erased
#define SHI_ERASED_WORD 0xFFFFFFFF

/// If defined enables the prefixes $, # and % for numbers
#define SHI_ENABLE_NUMBER_PREFIX 1

/// If defined, printing messages is enabled and used for errors
#define SHI_ENABLE_PRINT 1

// clang-format off
#define SHI_ENABLE_STORE           1
#define SHI_ENABLE_NUM             0
#define SHI_ENABLE_NUM_END         0
#define SHI_ENABLE_NUM_S           0
#define SHI_ENABLE_TICK            1
#define SHI_ENABLE_P               0
#define SHI_ENABLE_TIMES           1
#define SHI_ENABLE_TIMES_DIV       1
#define SHI_ENABLE_TIMES_DIV_MOD   0
#define SHI_ENABLE_PLUS            1
#define SHI_ENABLE_PLUS_STORE      1
#define SHI_ENABLE_PLUS_LOOP       1
#define SHI_ENABLE_COMMA           1
#define SHI_ENABLE_MINUS           1
#define SHI_ENABLE_D               0
#define SHI_ENABLE_DOT_Q           0
#define SHI_ENABLE_DIV             1
#define SHI_ENABLE_DIV_MOD         0
#define SHI_ENABLE_ZERO_LESS       1
#define SHI_ENABLE_ZERO_EQUAL      1
#define SHI_ENABLE_ONE_PLUS        1
#define SHI_ENABLE_ONE_MINUS       1
#define SHI_ENABLE_TWO_STORE       1
#define SHI_ENABLE_TWO_TIMES       1
#define SHI_ENABLE_TWO_DIV         1
#define SHI_ENABLE_TWO_FETCH       1
#define SHI_ENABLE_TWO_DROP        1
#define SHI_ENABLE_TWO_DUP         1
#define SHI_ENABLE_TWO_OVER        1
#define SHI_ENABLE_TWO_SWAP        1
#define SHI_ENABLE_COLON           1
#define SHI_ENABLE_SEMI            1
#define SHI_ENABLE_LESS            1
#define SHI_ENABLE_NUM_START       0
#define SHI_ENABLE_EQUAL           1
#define SHI_ENABLE_MORE            1
#define SHI_ENABLE_TO_BODY         1
#define SHI_ENABLE_TO_IN           1
#define SHI_ENABLE_TO_NUMBER       0
#define SHI_ENABLE_TO_R            1
#define SHI_ENABLE_Q_DUP           1
#define SHI_ENABLE_FETCH           1
#define SHI_ENABLE_ABORT           0
#define SHI_ENABLE_ABORT_Q         0
#define SHI_ENABLE_ABS             1
#define SHI_ENABLE_ACCEPT          0
#define SHI_ENABLE_ALIGN           1
#define SHI_ENABLE_ALIGNED         1
#define SHI_ENABLE_ALLOT           1
#define SHI_ENABLE_AND             1
#define SHI_ENABLE_BASE            1
#define SHI_ENABLE_BEGIN           1
#define SHI_ENABLE_BL              0
#define SHI_ENABLE_C_STORE         1
#define SHI_ENABLE_C_COMMA         1
#define SHI_ENABLE_C_FETCH         1
#define SHI_ENABLE_CELL_PLUS       1
#define SHI_ENABLE_CELLS           1
#define SHI_ENABLE_CHAR            0
#define SHI_ENABLE_CHAR_PLUS       1
#define SHI_ENABLE_CHARS           1
#define SHI_ENABLE_CONSTANT        1
#define SHI_ENABLE_COUNT           0
#define SHI_ENABLE_CR              0
#define SHI_ENABLE_CREATE          1
#define SHI_ENABLE_DECIMAL         1
#define SHI_ENABLE_DEPTH           1
#define SHI_ENABLE_DO              1
#define SHI_ENABLE_DOES            1
#define SHI_ENABLE_DROP            1
#define SHI_ENABLE_DUP             1
#define SHI_ENABLE_ELSE            1
#define SHI_ENABLE_EMIT            0
#define SHI_ENABLE_ENVIRONMENT_Q   0
#define SHI_ENABLE_EVALUATE        1
#define SHI_ENABLE_EXECUTE         1
#define SHI_ENABLE_EXIT            1
#define SHI_ENABLE_FILL            0
#define SHI_ENABLE_FIND            1
#define SHI_ENABLE_FM_DIV_MOD      0
#define SHI_ENABLE_HERE            1
#define SHI_ENABLE_HOLD            0
#define SHI_ENABLE_I               1
#define SHI_ENABLE_IF              1
#define SHI_ENABLE_IMMEDIATE       0
#define SHI_ENABLE_INVERT          1
#define SHI_ENABLE_J               1
#define SHI_ENABLE_KEY             0
#define SHI_ENABLE_LEAVE           1
#define SHI_ENABLE_LITERAL         1
#define SHI_ENABLE_LOOP            1
#define SHI_ENABLE_LSHIFT          1
#define SHI_ENABLE_M_TIMES         0
#define SHI_ENABLE_MAX             1
#define SHI_ENABLE_MIN             1
#define SHI_ENABLE_MOD             1
#define SHI_ENABLE_MOVE            0
#define SHI_ENABLE_NEGATE          1
#define SHI_ENABLE_OR              1
#define SHI_ENABLE_OVER            1
#define SHI_ENABLE_POSTPONE        1
#define SHI_ENABLE_QUIT            0
#define SHI_ENABLE_R_FROM          1
#define SHI_ENABLE_R_FETCH         1
#define SHI_ENABLE_RECURSE         1
#define SHI_ENABLE_REPEAT          1
#define SHI_ENABLE_ROT             1
#define SHI_ENABLE_RSHIFT          1
#define SHI_ENABLE_S_Q             0
#define SHI_ENABLE_S_TO_D          0
#define SHI_ENABLE_SIGN            0
#define SHI_ENABLE_SM_DIV_REM      0
#define SHI_ENABLE_SOURCE          1
#define SHI_ENABLE_SPACE           0
#define SHI_ENABLE_SPACES          0
#define SHI_ENABLE_STATE           1
#define SHI_ENABLE_SWAP            1
#define SHI_ENABLE_THEN            1
#define SHI_ENABLE_TYPE            0
#define SHI_ENABLE_U_D             0
#define SHI_ENABLE_U_LESS          1
#define SHI_ENABLE_UM_TIMES        0
#define SHI_ENABLE_UM_DIV_MOD      0
#define SHI_ENABLE_UNLOOP          1
#define SHI_ENABLE_UNTIL           1
#define SHI_ENABLE_VARIABLE        1
#define SHI_ENABLE_WHILE           1
#define SHI_ENABLE_WORD            0
#define SHI_ENABLE_XOR             1
#define SHI_ENABLE_BRACKET_LEFT    1
#define SHI_ENABLE_BRACKET_TICK    1
#define SHI_ENABLE_BRACKET_CHAR    0
#define SHI_ENABLE_BRACKET_RIGHT   1

#define SHI_ENABLE_DOT_COMMENT     0
#define SHI_ENABLE_DOT_R           0
#define SHI_ENABLE_ZERO_NE         1
#define SHI_ENABLE_ZERO_MORE       1
#define SHI_ENABLE_TWO_TO_R        1
#define SHI_ENABLE_TWO_R_FROM      1
#define SHI_ENABLE_TWO_R_FETCH     1
#define SHI_ENABLE_COLON_NONAME    0
#define SHI_ENABLE_NE              1
#define SHI_ENABLE_Q_DO            0
#define SHI_ENABLE_ACTION_OF       0
#define SHI_ENABLE_AGAIN           1
#define SHI_ENABLE_BUFFER_COLON    0
#define SHI_ENABLE_C_Q             0
#define SHI_ENABLE_CASE            1
#define SHI_ENABLE_COMPILE_COMMA   1
#define SHI_ENABLE_DEFER           0
#define SHI_ENABLE_DEFER_STORE     0
#define SHI_ENABLE_DEFER_FETCH     0
#define SHI_ENABLE_ENDCASE         1
#define SHI_ENABLE_ENDOF           1
#define SHI_ENABLE_ERASE           0
#define SHI_ENABLE_FALSE           1
#define SHI_ENABLE_HEX             1
#define SHI_ENABLE_HOLDS           0
#define SHI_ENABLE_IS              0
#define SHI_ENABLE_MARKER          0
#define SHI_ENABLE_NIP             1
#define SHI_ENABLE_OF              1
#define SHI_ENABLE_PAD             0
#define SHI_ENABLE_PARSE           1
#define SHI_ENABLE_PARSE_NAME      0
#define SHI_ENABLE_PICK            1
#define SHI_ENABLE_REFILL          0
#define SHI_ENABLE_RESTORE_INPUT   0
#define SHI_ENABLE_ROLL            1
#define SHI_ENABLE_S_EQ            0
#define SHI_ENABLE_SAVE_INPUT      0
#define SHI_ENABLE_SOURCE_ID       0
#define SHI_ENABLE_TO              0
#define SHI_ENABLE_TRUE            1
#define SHI_ENABLE_TUCK            1
#define SHI_ENABLE_U_DOT_R         0
#define SHI_ENABLE_U_MORE          1
#define SHI_ENABLE_UNUSED          1
#define SHI_ENABLE_VALUE           0
#define SHI_ENABLE_WITHIN          0
#define SHI_ENABLE_BRACKET_COMPILE 0
#define SHI_ENABLE_BS              0

#define SHI_ENABLE_BINARY          1
#define SHI_ENABLE_C_VARIABLE      1
#define SHI_ENABLE_TO_TEXT_Q       1
#define SHI_ENABLE_TO_DATA_Q       1
#define SHI_ENABLE_TO_TEXT         1
#define SHI_ENABLE_TO_DATA         1
// clang-format on

// C/C++
#ifndef __ASSEMBLER__

#  include <stddef.h>
#  include <stdint.h>
#  include <string.h>

#  ifdef __cplusplus
extern "C" {
#  endif

typedef void (*void_fp)();

extern void_fp shi_tick_asm(char const* str, size_t len);
extern void shi_clear_asm();
extern void shi_c_variable_asm(char const* name, size_t len);
extern void shi_evaluate_asm(char const* str, size_t len);
extern uint32_t s_shi_context;

#  ifdef __cplusplus
}
#  endif

// C only
#  ifndef __cplusplus

typedef struct {
  uint32_t data_begin;
  uint32_t data_end;
  uint32_t text_begin;
  uint32_t text_end;
  uint8_t text_p2align;
} shi_init_t;

void shi_init_asm(shi_init_t*);

inline void shi_init(shi_init_t s) {
  shi_init_asm(&s);
}

/// Call of word evaluate
///
/// \param  str String address
///
/// \dot
/// digraph G {
///
///   ratio=auto; node[fontsize=12]; newrank=true;
///
///   # Check cstring lenght in forthEvaluate
///   check_cstring_length->return[label="no + error"];
///   check_cstring_length->enter[label="yes"];
///   check_cstring_length[shape=diamond, label="cstring length > 0"];
///
///   # Enter forth
///   enter[label="restore forth context\npush cstring\ncall evaluate"];
///   enter->evaluate;
///   evaluate[label="store source\nset source-id -1\nset >in 0\ncall
///   interpret"];
///
///   # Parse
///   evaluate->parse;
///   parse[label="call source\ncall parse"];
///   parse->check_token;
///
///   # Find
///   check_token[shape=diamond, label="token length > 0"];
///   check_token->return[label="no + error"];
///   check_token->find[label="yes"];
///   find[label="call find"];
///   find->check_find;
///   check_find[shape=diamond, label="found token in dictionary"];
///   check_find->number[label="no"];
///
///   # Number
///   number[label="call number"];
///   check_find->check_state[label="yes"];
///   check_state[shape=diamond, label="state == 0"];
///   number->check_number;
///   check_number[shape=diamond, label="number found"];
///   check_number->return[label="no + error"];
///   check_number->push_number[label="yes"];
///   push_number[label="push number"];
///   push_number->check_literal_folding;
///   check_literal_folding[shape=diamond, label="literal-folding pointer ==
///   0"]; check_literal_folding->set_literal_folding[label="yes"];
///   set_literal_folding[label="literal-folding pointer =\nparameter stack
///   pointer"]; set_literal_folding->check_done;
///   check_literal_folding->check_done[label="no"];
///
///   # Done
///   check_done[shape=diamond, label=">in < cstring length"];
///   check_done->return[label="no"];
///   check_done->parse[label="yes"];
///
///   # State
///   check_state->interpret[label="yes"];
///   check_state->compile[label="no"];
///
///   # Interpret
///   interpret[shape=diamond, label="word interpretable"];
///   interpret->return[label="no + error"];
///   interpret->execute_interpret[label="yes"];
///   execute_interpret[label="literal-folding pointer = 0\ncall execute"];
///   execute_interpret->check_done;
///
///   # Compile
///   compile[shape=diamond, label="word compileable"];
///   compile->return[label="no + error"];
///   compile->check_lfp[label="yes"];
///
///   # Constant-folding pointer
///   check_lfp[shape=diamond, label="literal-folding pointer == 0"];
///   check_lfp->check_immediate[label="yes"];
///   check_lfp->calculate_literals[label="no"];
///   calculate_literals[label="get number of literals\nand folds bits"];
///   calculate_literals->check_foldable; check_foldable[shape=diamond,
///   label="word foldable"]; check_foldable->check_folds[label="yes"];
///   check_foldable->literal_comma[label="no"];
///   check_folds[shape=diamond, label="number of literals > folds bits\n(this
///   means we can safely optimize\nliterals and a following word\nwithout loss
///   of information)"]; check_folds->execute_compile[label="yes"];
///   execute_compile[label="call execute"];
///   execute_compile->check_done;
///   check_folds->literal_comma[label="no"];
///   literal_comma[label="call literal_comma"];
///   literal_comma->check_immediate;
///
///   # Immediate
///   check_immediate[shape=diamond, label="word immediate"];
///   check_immediate->execute_compile[label="yes"];
///   check_immediate->check_inline[label="no"];
///
///   check_inline[shape=diamond, label="word inlineable"];
///   check_inline->inline_comma[label="yes"];
///   check_inline->bl_comma[label="yes"];
///   inline_comma[label="call inline_comma"];
///   inline_comma->check_done;
///   bl_comma[label="call bl_comma"];
///   bl_comma->check_done;
/// }
/// \enddot
inline void shi_evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

inline void shi_evaluate_len(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

inline void shi_cvariable(char const* str, void* adr) {
  push(reinterpret_cast<int32_t>(adr));
  shi_c_variable_asm(str, strlen(name));
}

// C++ only
#  else

#    include <cstdio>
#    include <tuple>
#    include <type_traits>

namespace shi {

template<typename T>
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<typename T>
struct is_reference_wrapper : std::false_type {};

template<typename U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};

template<typename T>
inline constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

struct init_t {
  uint32_t data_begin{};
  uint32_t data_end{};
  uint32_t text_begin{};
  uint32_t text_end{};
  uint8_t text_p2align{2};
};

extern "C" void shi_init_asm(init_t&);

inline void init(init_t s) {
  shi_init_asm(s);
}

inline void clear() {
  shi_clear_asm();
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
inline size_t depth() {
  size_t size;

  asm volatile("ldr r0, [%1, #4] \n"
               "subs %0, %1, r0 \n"
               "lsrs %0, %0, #2 \n"
               : "=r"(size)
               : "r"(&s_shi_context)
               : "cc", "memory", "r0");

  return size;
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
inline size_t size() {
  return depth();
}

/// Add element to the top of the stack
///
/// \param    cell    Value to push
template<typename T>
inline void push(T&& t) {
  using std::addressof;
  using std::is_arithmetic_v;
  using std::is_pointer_v;
  using V = remove_cvref_t<T>;

  static_assert(sizeof(V) <= SHI_STACK_SIZE * 4);

  // every built-in stuff, pointers and reference_wrappers
  if constexpr (sizeof(V) <= 4 && (is_arithmetic_v<V> || is_pointer_v<V> ||
                                   is_reference_wrapper_v<V>))
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%0] \n"
                 "str tos, [dsp, #-4]! \n"
                 "movs tos, %1 \n"
                 "strd tos, dsp, [%0] \n"
                 :
                 : "r"(&s_shi_context), "r"(t)
                 : "cc", "memory", "r0", "r1");
  // doubles, int64 and uint64
  else if constexpr (sizeof(V) == 8 && is_arithmetic_v<V>)
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%0] \n"
                 "ldrd r2, r3, [%1] \n"
                 "strd r2, tos, [dsp, #-8]! \n"
                 "movs tos, r3 \n"
                 "strd tos, dsp, [%0] \n"
                 :
                 : "r"(&s_shi_context), "r"(addressof(t))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  // everything else
  else
    asm volatile("nop");  // basically memcpy here?
}

template<typename T>
inline remove_cvref_t<T> pop() {
  using std::addressof;
  using std::is_arithmetic_v;
  using std::is_pointer_v;
  using V = remove_cvref_t<T>;

  T t;

  if constexpr (sizeof(V) <= 4 && (is_arithmetic_v<V> || is_pointer_v<V> ||
                                   is_reference_wrapper_v<V>))
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%1] \n"
                 "movs %0, tos \n"
                 "ldmia dsp!, {tos} \n"
                 "strd tos, dsp, [%1] \n"
                 : "=r"(t)
                 : "r"(&s_shi_context)
                 : "cc", "memory", "r0", "r1");
  else if constexpr (sizeof(V) == 8 && is_arithmetic_v<V>)
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%0] \n"
                 "ldrd r2, r3, [dsp], #8 \n"
                 "strd r2, tos, [%1] \n"
                 "movs tos, r3 \n"
                 "strd tos, dsp, [%0] \n"
                 :
                 : "r"(&s_shi_context), "r"(addressof(t))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  else
    asm volatile("nop");  // basically memcpy here?

  return t;
}

/// Removes first element
inline void pop() {
  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "ldmia dsp!, {tos} \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&s_shi_context)
               : "cc", "memory", "r0", "r1");
}

inline int32_t top(size_t offset = 0) {
  int32_t cell;

  asm volatile("cmp %2, #0 \n"
               "iteee eq \n"
               "ldreq %0, [%1] \n"
               "subne r1, %2, #1 \n"
               "ldrne r0, [%1, #4] \n"
               "ldrne %0, [r0, r1, lsl #2] \n"
               : "=r"(cell)
               : "r"(&s_shi_context), "r"(offset)
               : "cc", "memory", "r0", "r1");

  return cell;
}

inline void evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Call of word evaluate
///
/// \param  str String address
/// \param  len String length
inline void evaluate(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

inline void operator"" _s(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

template<typename T>
void cvariable(char const* str, T adr) {
  static_assert(std::is_pointer_v<T>);
  static_assert(std::is_integral_v<std::remove_pointer_t<T>>);

  push(reinterpret_cast<int32_t>(adr));
  shi_c_variable_asm(str, strlen(str));
}

}  // namespace shi

#  endif

#endif
