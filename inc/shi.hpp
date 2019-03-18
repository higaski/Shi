/// Shi
///
/// \file   shi.hpp
/// \author	Vincent Hamp
/// \date   27/07/2016

/// \mainpage Shi
/// Shi is an unusual Forth in a sense that it's not a full-fledged
/// self-contained system. Neither
/// [key](http://forth-standard.org/standard/core/KEY)/[emit](http://forth-standard.org/standard/core/EMIT),
/// which deal with user input, nor any of the string conversion words (e.g.
/// [#](https://forth-standard.org/standard/core/num), ...) are implemented.
/// Instead of a
/// [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop)
/// Shi is intended to be used as embedded scripting language by featuring an
/// extensive C/C++ API. The functions shi::evaluate and shi_evaluate can be
/// used to enter the classic interpretation loop. This reflects what word
/// [evaluate](https://forth-standard.org/standard/core/EVALUATE) does.
///
/// \dot
/// digraph G {
///
///   subgraph cluster_c_cpp {
///     style="dotted";
///     labeljust="l";
///     label = "C/C++";
///     shi_evaluate;
///   }
///
///   subgraph cluster_forth {
///     style="dotted";
///     labeljust="l";
///     label = "Forth";
///     word[shape=diamond, label="word?"];
///     execute_compile[label="execute/compile"];
///     number[shape=diamond, label="number?"];
///     push_literal[label="push/literal"];
///     ambiguous_condition[label="ambiguous condition"];
///     done[shape=diamond, label="done?"];
///     word->execute_compile[label="yes"];
///     word->number[label="no"];
///     execute_compile->done;
///     number->push_literal[label="yes"];
///     number->ambiguous_condition[label="no"];
///     push_literal->done;
///     ambiguous_condition->done;
///     done->word[label="no"];
///   }
///
///   shi_evaluate->word;
///   done->shi_evaluate[label="yes", constraint=false];
///
/// }
/// \enddot
///
///
/// \page page_implementation Implementation
/// Although Forth systems are usually rather simple to develop there are still
/// topics which should be covered in some detail. This applies above all to
/// things that are not enforced by the standard such as
/// - The actual quantity and implementation of data-, control- and return-stack
/// - Dictionary entry layouts and word flags
/// - Relationship between code- and data space
/// - Action on an ambiguous condition
///
/// and of course to non-standard additions.
///
// clang-format off
/// \page page_implementation Implementation
/// | Page                      | Content                                      |
/// | ------------------------- | -------------------------------------------- |
/// | \subpage page_stacks      | Handling of data-, control-flow- and return-stack |
/// | \subpage page_variables   | Internally used variables             |
/// | \subpage page_dictionary  | Structure of core and user dictionary |
/// | \subpage page_eval        | Evaluate loop in detail |
/// | \subpage page_todo        | d |
/// | \subpage page_control_structures | TODO |
/// | \subpage page_doc_stuff   | TODO |
/// | \subpage page_lerp        | c |
// clang-format on
///
/// \page page_stacks Stacks
/// Forth has the notation of three different stacks:
/// -# Data-stack for passing values
/// -# Control-flow-stack for storing jump origins and destinations for
///    conditional branches
/// -# Return-stack for do...loop constructs and temporary storage
///
/// Shi only uses two physical stacks, the data- and the return-stack. This is
/// possible because all control-flow words (e.g.
/// [if](https://forth-standard.org/standard/core/IF),
/// [else](https://forth-standard.org/standard/core/ELSE),
/// [then](https://forth-standard.org/standard/core/THEN),
/// [begin](https://forth-standard.org/standard/core/BEGIN),
/// [until](https://forth-standard.org/standard/core/UNTIL),
/// [while](https://forth-standard.org/standard/core/WHILE), ...) are so called
/// "compile only" words which means that they can only be used when creating a
/// new definition. The way the standard phrases this is that interpretation
/// semantics for these word are undefined.
///
/// So this works
///
/// ```
/// : lunch? if eat then ;
/// ```
///
/// whereas this doesn't.
///
/// ```
/// lunch? if eat then
/// ```
///
/// The principal difference is that during compilation mode inside the
/// colon-definition the user can't alter the data-stack which makes it
/// perfectly safe to store branching information on it.
///
/// **Data-stack**<br>
/// The data-stack in Shi is explicit. Two physical registers are reserved for
/// it. There is *tos* which is short for top-of-stack and *dsp* which is the
/// data-stack-pointer. Upon pushing to or popping from the stack values are
/// stored or loaded at the address in *dsp*.
///
/// ```
/// tos .req r6
/// dsp .req r7
/// ```
///
/// The stack size itself can be adjusted with the macro \ref SHI_STACK_SIZE.
///
/// **Control-flow-stack**<br>
/// Usually control-flow words have a 1:1 relation between the word that creates
/// a branch and the word that has to resolve it.
/// [forth-standard.org](https://forth-standard.org/) offers a very nice visual
/// representation of control-flow words
/// [here](https://forth-standard.org/standard/rationale). The problem is that
/// there are two exceptions from this 1:1 relation which are
/// [endof](https://forth-standard.org/standard/core/ENDOF) and
/// [leave](https://forth-standard.org/standard/core/LEAVE). Those words can
/// nest an arbitrary number of times and even mix with other control-flow
/// words. To avoid any clashes
/// [endof](https://forth-standard.org/standard/core/ENDOF) and
/// [leave](https://forth-standard.org/standard/core/LEAVE) push their
/// control-flow values by using the control-stack-pointer. *csp* touches the
/// same physical memory as *dsp* but pushes from stack begin towards the end.
/// Since *csp* is less commonly used it's only stored as variable.
///
/// **Return-stack**<br>
/// The return-stack is implicit and shared with the main application. This
/// means that the return-stack is basically whatever the stack-pointer of the
/// ARMv7-M architecture points to. Use of return-stack is generally discouraged
/// as incautious use can not only crash Shi but the entire system.
///
/// \page page_variables Variables
/// There are a couple of states Shi needs to track internally. When evaluating
/// the total ram usage these variables have to be taken into consideration and
/// added to the user-defined stack size. The table below contains them all
/// including a short description and their size in bytes.
///
// clang-format off
/// \page page_variables Variables
/// | Symbols         | Responsibility                                                                     | Size [b]     |
/// | ----------------| ---------------------------------------------------------------------------------- | ------------ |
/// | shi_stack_begin | Stack begin                                                                        | user-defined |
/// | shi_stack_end   | Stack end                                                                          |              |
/// | shi_context     | Context (tos, dsp and lfp)                                                         | 12           |
/// |                 |                                                                                    |              |
/// | to_text_begin   | Pointer to current ram begin which is going to end in flash                        | 4            |
/// | data_begin      | Pointer to ram begin                                                               | 4            |
/// | data_end        | Pointer to ram end, used for reserving ram for variables                           | 4            |
/// | text_begin      | Pointer to flash begin                                                             | 4            |
/// | text_end        | Pointer to flash end                                                               | 4            |
/// |                 |                                                                                    |              |
/// | csp             | Inside loop: points to leave addresses from the current loop on the stack <br><!-- |              |
/// |                 | --> Inside case: points to endof addresses from the current case on the stack      | 4            |
/// | link            | Contains address of link of the last definition                                    | 4            |
/// | status          | Current state (state is taken as word) <br><!--                                    |              |
/// |                 | --> false: interpret, true: compile                                                | 4            |
/// | src             | Source (address and length)                                                        | 8            |
/// | in              | Index in terminal input buffer                                                     | 4            |
/// | radix           | Determine current numerical base (base is taken as word)                           | 4            |
/// | text_align      | User defined alignment for flash                                                   | 1            |
// clang-format on
///
/// Most variables should be more or less evident but some are most likely not:
///
/// \page page_variables Variables
/// **shi_context**<br>
/// Shi maps three pointers directly to registers which must be restored and
/// saved upon entry and exit. Those registers are the top-of-stack, the
/// data-space-pointer and the literal-folding-pointer. shi_context marks the
/// storage for these registers.
///
/// **data_begin, data_end, text_begin, text_end**<br>
/// Usually Forth system have a single continuous data-space but Shi supports
/// compilation to two different data-spaces (data and text aka ram and flash).
/// The variables data_begin, data_end, text_begin and text_end keep track of
/// the data-spaces passed in by the user at initialization.
///
/// **to_text_begin**<br>
/// Shi does not write directly to flash but first stores new definitions in ram
/// before a special word calls an extern function which then takes care of
/// copying the definitions over. to_text_begin marks the beginning of the
/// memory which should be written into flash.
///
/// **text_align**<br>
/// Most flash memories have alignment restrictions and can only write to 4-, 8-
/// or even 16-byte aligned addresses. This alignment requirements are stored in
/// text_align and can be set by the user at initialization.
///
/// \page page_dictionary Dictionary
/// single entry
///
/// \dot
/// digraph H {
///
///   aHtmlTable [
///     shape=plaintext
///     label=<
///
///     <table border='1' cellborder='0'>
///       <tr><td>link</td><td>foo</td></tr>
///       <tr><td>flags</td><td>bar</td></tr>
///       <tr><td>name</td><td>bar</td></tr>
///       <tr><td>data/code</td><td>bar</td></tr>
///     </table>>
///
///   ];
///
/// }
/// \enddot
///
/// Linked list
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
// Enable words individually
// Core words
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

// Extension words
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

// Own words
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
extern void shi_evaluate_asm(char const* str, size_t len);
extern void shi_clear_asm();
extern void_fp shi_tick_asm(char const* str, size_t len);
extern void shi_variable_asm(char const* name, size_t len);
extern uint32_t shi_context;

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

/// Initialize
///
/// \param  s   Init structure
inline void shi_init(shi_init_t s) {
  shi_init_asm(&s);
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
inline void shi_evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
inline void shi_evaluate_len(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

inline void shi_variable(char const* str, void* adr) {
  push(reinterpret_cast<int32_t>(adr));
  shi_variable_asm(str, strlen(name));
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

/// Initialize
///
/// \param  s   Init structure
inline void init(init_t s) {
  shi_init_asm(s);
}

/// Clear stack
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
               : "r"(&shi_context)
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
/// \tparam T   Type of element to push
/// \param  t   Value to push
template<typename T>
inline void push(T&& t) {
  using std::addressof;
  using std::is_arithmetic_v;
  using std::is_pointer_v;
  using V = remove_cvref_t<T>;

  static_assert(sizeof(V) <= SHI_STACK_SIZE * 4);

  if constexpr (sizeof(V) <= 4 && (is_arithmetic_v<V> || is_pointer_v<V> ||
                                   is_reference_wrapper_v<V>))
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%0] \n"
                 "str tos, [dsp, #-4]! \n"
                 "movs tos, %1 \n"
                 "strd tos, dsp, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(t)
                 : "cc", "memory", "r0", "r1");
  else if constexpr (sizeof(V) == 8 && is_arithmetic_v<V>)
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%0] \n"
                 "ldrd r2, r3, [%1] \n"
                 "strd r2, tos, [dsp, #-8]! \n"
                 "movs tos, r3 \n"
                 "strd tos, dsp, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  else
    asm volatile("nop");  // basically memcpy here?
}

/// Remove element from the top of the stack
///
/// \tparam T   Type of element to pop
/// \return Value
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
                 : "r"(&shi_context)
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
                 : "r"(&shi_context), "r"(addressof(t))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  else
    asm volatile("nop");  // basically memcpy here?

  return t;
}

/// Remove element from the top of the stack
inline void pop() {
  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "ldmia dsp!, {tos} \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&shi_context)
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
               : "r"(&shi_context), "r"(offset)
               : "cc", "memory", "r0", "r1");

  return cell;
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
inline void evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
inline void evaluate(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

inline void operator"" _s(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

template<typename T>
void variable(char const* str, T adr) {
  static_assert(std::is_pointer_v<T>);
  static_assert(std::is_integral_v<std::remove_pointer_t<T>>);

  push(adr);
  shi_variable_asm(str, strlen(str));
}

}  // namespace shi

#  endif

#endif
