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
#  define SHI_DSTACK_SIZE 256
#  if SHI_DSTACK_SIZE % 4
#    error SHI_DSTACK_SIZE must be a multiple of 4
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

void shi_c_variable_asm(char const* name, size_t len);
void shi_evaluate_asm(char const* str, size_t len);

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
  asm volatile("ldr r0, =_s_shi_dstack"
               "\n\t"
               "movs r1, #0"
               "\n\t"
               "ldr r2, =_e_shi_dstack"
               "\n\t"

               "1:"
               "\n\t"
               "cmp r0, r2"
               "\n\t"
               "beq 2f"
               "\n\t"
               "str r1, [r0], #4"
               "\n\t"
               "b   1b"
               "\n\t"

               "2:"
               "\n\t"
               "movs r1, #'*'"
               "\n\t"
               "ldr r0, =_s_shi_context"
               "\n\t"
               "stmia r0, {r1, r2}"
               "\n\t"
               ".ltorg"
               "\n\t"
               :
               :
               : "cc", "r0", "r1", "r2");
}

/// Returns true if the stack is empty
///
/// \return true    Stack empty
/// \return false   Stack not empty
inline bool empty() {
  bool flag;

  asm volatile("ldr r0, =_e_shi_dstack"
               "\n\t"
               "ldr %[flag], =_s_shi_context+4"
               "\n\t"
               "ldr %[flag], [%[flag]]"
               "\n\t"
               "cmp %[flag], r0"
               "\n\t"
               "ite eq"
               "\n\t"
               "moveq %[flag], #1"
               "\n\t"
               "movne %[flag], #0"
               "\n\t"
               ".ltorg"
               "\n\t"
               : [flag] "=r"(flag)
               :
               : "cc", "r0");

  return flag;
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
inline size_t depth() {
  size_t size;

  asm volatile("ldr r0, =_e_shi_dstack"
               "\n\t"
               "ldr %[size], =_s_shi_context+4"
               "\n\t"
               "ldr %[size], [%[size]]"
               "\n\t"
               "rsb %[size], r0"
               "\n\t"
               "lsrs %[size], #2"
               "\n\t"
               ".ltorg"
               "\n\t"
               : [size] "=r"(size)
               :
               : "cc", "r0");

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
  asm volatile("ldr r0, =_s_shi_context"
               "\n\t"
               "ldmia r0, {r1, r2}"
               "\n\t"
               "str r1, [r2, #-4]!"
               "\n\t"
               "movs r1, %[cell]"
               "\n\t"
               "stmia r0, {r1, r2}"
               "\n\t"
               ".ltorg"
               "\n\t"
               :
               : [cell] "r"(cell)
               : "cc", "r0", "r1", "r2");
}

/// Removes first element
inline void pop() {
  asm volatile("ldr r0, =_s_shi_context"
               "\n\t"
               "ldmia r0, {r1, r2}"
               "\n\t"
               "ldmia r2!, {r1}"
               "\n\t"
               "stmia r0, {r1, r2}"
               "\n\t"
               ".ltorg"
               "\n\t"
               :
               :
               : "cc", "r0", "r1", "r2");
}

inline int32_t top(int32_t offset = 0) {
  int32_t cell;

  asm volatile("ldr r7, =_s_shi_context"
               "\n\t"
               "ldmia r7, {%[cell], r7}"
               "\n\t"

               "cmp %[offset], #0"
               "\n\t"
               "beq 1f"
               "\n\t"

               "itee ge"
               "\n\t"
               "subge %[offset], #1"
               "\n\t"
               "addlt %[offset], #1"
               "\n\t"
               "neglt %[offset], %[offset]"
               "\n\t"

               "ldr %[cell], [r7, %[offset], lsl #2]"
               "\n\t"

               "1:"
               "\n\t"

               ".ltorg"
               "\n\t"
               : [cell] "=&r"(cell)
               : [offset] "r"(offset)
               : "cc", "r7");

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

/// Print stack elements with fp
///
/// \param  fp      Print function
/// \return Number of characters written if successful or a negative value if an
///         error occurred
inline int stack_print(int (*fp)(char const* const format, ...)) {
  struct {
    float f;
    int32_t i;
  } cell;

  uint32_t n{depth()};

  if (!n)
    fp("\n*** Forth stack empty ***\n");
  else
    fp("\n*** Forth stack has %d elements ***\n", n);

  for (auto i{0u}; i < n; i++) {
    asm volatile("cmp %[i], #0"
                 "\n\t"
                 "bne 1f"
                 "\n\t"

                 "ldr %[cell_i], =_s_shi_context"
                 "\n\t"
                 "ldr %[cell_i], [%[cell_i]]"
                 "\n\t"
                 "b 2f"
                 "\n\t"

                 "1:"
                 "\n\t"
                 "subs r0, %[i], 1"
                 "\n\t"
                 "ldr %[cell_i], =_s_shi_context+4"
                 "\n\t"
                 "ldr %[cell_i], [%[cell_i]]"
                 "\n\t"
                 "ldr %[cell_i], [%[cell_i], r0, lsl #2]"
                 "\n\t"

                 "2:"
                 "\n\t"
                 "movs %[cell_f], %[cell_i]"
                 "\n\t"
                 : [cell_f] "=r"(cell.f), [cell_i] "=r"(cell.i)
                 : [i] "r"(i)
                 : "cc", "r0");

    if (!i)
      fp("tos:\t\t%d\t\t%f\n", cell.i, cell.f);
    else
      fp("%d:\t\t%d\t\t%f\n", i, cell.i, cell.f);
  }

  return 0;
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
