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

// Assembler only
#ifdef __ASSEMBLER__

/// Data-stack size in bytes
#  define SHI_STACK_SIZE 256
#  if SHI_STACK_SIZE % 4
#    error SHI_STACK_SIZE must be a multiple of 4
#  endif

/// Most flash types are 0xFF erased
#  define SHI_ERASED_WORD 0xFFFFFFFF

/// If defined enables the prefixes $, # and % for numbers
#  define SHI_NUMBER_PREFIX 1

/// If defined, printing messages is enabled and used for errors
#  define SHI_PRINT_ENABLED 1

// C/C++
#else

#  include <stddef.h>
#  include <stdint.h>
#  include <string.h>

#  ifdef __cplusplus
extern "C" {
#  endif

extern void shi_clear_asm();
extern void shi_c_variable_asm(char const* name, size_t len);
extern void shi_evaluate_asm(char const* str, size_t len);
extern uint32_t s_shi_stack;
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

void shi_init_asm(init_t*);

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
#    include <type_traits>

namespace shi {

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
inline void push(int32_t cell) {
  asm volatile("ldmia %0, {r0, r1} \n"
               "str r0, [r1, #-4]! \n"
               "movs r0, %1 \n"
               "stmia %0, {r0, r1} \n"
               :
               : "r"(&s_shi_context), "r"(cell)
               : "cc", "memory", "r0", "r1");
}

/// Removes first element
inline void pop() {
  asm volatile("ldmia %0, {r0, r1} \n"
               "ldmia r1!, {r0} \n"
               "stmia %0, {r0, r1} \n"
               :
               : "r"(&s_shi_context)
               : "cc", "memory", "r0", "r1");
}

inline int32_t top(int32_t offset = 0) {
  int32_t cell;

  asm volatile("eor r0, %2, %2, asr #31 \n"
               "sub r0, r0, %2, asr #31 \n"
               "cmp r0, #0 \n"
               "iteee eq \n"
               "ldreq %0, [%1] \n"
               "subne r0, #1 \n"
               "ldrne %0, [%1, #4] \n"
               "ldrne %0, [%0, r0, lsl #2] \n"
               : "=r"(cell)
               : "r"(&s_shi_context), "r"(offset)
               : "cc", "memory", "r0");

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

inline void operator"" _fs(char const* str, size_t len) {
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
