/// Shi
///
/// \file   shi.hpp
/// \author Vincent Hamp
/// \date   11/04/2019

#pragma once

// clang-format off
/// Data-stack size (must be a multiple of 4 bytes)
#define SHI_STACK_SIZE 256
#if SHI_STACK_SIZE % 4
#  error SHI_STACK_SIZE must be a multiple of 4 bytes
#endif

/// Most flash types are 0xFF erased
#define SHI_ERASED_WORD 0xFFFFFFFF

/// If defined enables the prefixes $, # and % for numbers
#define SHI_ENABLE_NUMBER_PREFIX 1

/// If defined, printing messages is enabled and used for errors
#define SHI_ENABLE_PRINT 1

///
#define SHI_ENABLE_ZERO_LENGTH_STRING_ERROR     1
#define SHI_ENABLE_STACKOVERFLOW_ERROR          1
#define SHI_ENABLE_UNDEFINED_WORD_ERROR         1
#define SHI_ENABLE_REDEFINED_WORD_ERROR         1
#define SHI_ENABLE_INTERPRET_COMPILE_ONLY_ERROR 1
#define SHI_ENABLE_COMPILE_INTERPRET_ONLY_ERROR 1
#define SHI_ENABLE_BRANCH_OFFSET_ERROR          1

// Enable words individually
// Core words
#define SHI_ENABLE_STORE         1
#define SHI_ENABLE_TICK          1
#define SHI_ENABLE_TIMES         1
#define SHI_ENABLE_TIMES_DIV     1
#define SHI_ENABLE_TIMES_DIV_MOD 1
#define SHI_ENABLE_PLUS          1
#define SHI_ENABLE_PLUS_STORE    1
#define SHI_ENABLE_PLUS_LOOP     1
#define SHI_ENABLE_COMMA         1
#define SHI_ENABLE_MINUS         1
#define SHI_ENABLE_DIV           1
#define SHI_ENABLE_DIV_MOD       1
#define SHI_ENABLE_ZERO_LESS     1
#define SHI_ENABLE_ZERO_EQUAL    1
#define SHI_ENABLE_ONE_PLUS      1
#define SHI_ENABLE_ONE_MINUS     1
#define SHI_ENABLE_TWO_STORE     1
#define SHI_ENABLE_TWO_TIMES     1
#define SHI_ENABLE_TWO_DIV       1
#define SHI_ENABLE_TWO_FETCH     1
#define SHI_ENABLE_TWO_DROP      1
#define SHI_ENABLE_TWO_DUP       1
#define SHI_ENABLE_TWO_OVER      1
#define SHI_ENABLE_TWO_SWAP      1
#define SHI_ENABLE_COLON         1
#define SHI_ENABLE_SEMI          1
#define SHI_ENABLE_LESS          1
#define SHI_ENABLE_EQUAL         1
#define SHI_ENABLE_MORE          1
#define SHI_ENABLE_TO_BODY       1
#define SHI_ENABLE_TO_IN         1
#define SHI_ENABLE_TO_R          1
#define SHI_ENABLE_Q_DUP         1
#define SHI_ENABLE_FETCH         1
#define SHI_ENABLE_ABORT         1
#define SHI_ENABLE_ABORT_Q       1
#define SHI_ENABLE_ABS           1
#define SHI_ENABLE_ALIGN         1
#define SHI_ENABLE_ALIGNED       1
#define SHI_ENABLE_ALLOT         1
#define SHI_ENABLE_AND           1
#define SHI_ENABLE_BASE          1
#define SHI_ENABLE_BEGIN         1
#define SHI_ENABLE_C_STORE       1
#define SHI_ENABLE_C_COMMA       1
#define SHI_ENABLE_C_FETCH       1
#define SHI_ENABLE_CELL_PLUS     1
#define SHI_ENABLE_CELLS         1
#define SHI_ENABLE_CHAR_PLUS     1
#define SHI_ENABLE_CHARS         1
#define SHI_ENABLE_CONSTANT      1
#define SHI_ENABLE_CREATE        1
#define SHI_ENABLE_DECIMAL       1
#define SHI_ENABLE_DEPTH         1
#define SHI_ENABLE_DO            1
#define SHI_ENABLE_DOES          1
#define SHI_ENABLE_DROP          1
#define SHI_ENABLE_DUP           1
#define SHI_ENABLE_ELSE          1
#define SHI_ENABLE_EXECUTE       1
#define SHI_ENABLE_EXIT          1
#define SHI_ENABLE_FILL          1
#define SHI_ENABLE_FIND          1
#define SHI_ENABLE_FM_DIV_MOD    1
#define SHI_ENABLE_HERE          1
#define SHI_ENABLE_I             1
#define SHI_ENABLE_IF            1
#define SHI_ENABLE_IMMEDIATE     1
#define SHI_ENABLE_INVERT        1
#define SHI_ENABLE_J             1
#define SHI_ENABLE_LEAVE         1
#define SHI_ENABLE_LITERAL       1
#define SHI_ENABLE_LOOP          1
#define SHI_ENABLE_LSHIFT        1
#define SHI_ENABLE_M_TIMES       1
#define SHI_ENABLE_MAX           1
#define SHI_ENABLE_MIN           1
#define SHI_ENABLE_MOD           1
#define SHI_ENABLE_MOVE          1
#define SHI_ENABLE_NEGATE        1
#define SHI_ENABLE_OR            1
#define SHI_ENABLE_OVER          1
#define SHI_ENABLE_POSTPONE      1
#define SHI_ENABLE_QUIT          1
#define SHI_ENABLE_R_FROM        1
#define SHI_ENABLE_R_FETCH       1
#define SHI_ENABLE_RECURSE       1
#define SHI_ENABLE_REPEAT        1
#define SHI_ENABLE_ROT           1
#define SHI_ENABLE_RSHIFT        1
#define SHI_ENABLE_S_TO_D        1
#define SHI_ENABLE_SIGN          1
#define SHI_ENABLE_SM_DIV_REM    1
#define SHI_ENABLE_SOURCE        1
#define SHI_ENABLE_STATE         1
#define SHI_ENABLE_SWAP          1
#define SHI_ENABLE_THEN          1
#define SHI_ENABLE_U_LESS        1
#define SHI_ENABLE_UM_TIMES      1
#define SHI_ENABLE_UM_DIV_MOD    1
#define SHI_ENABLE_UNLOOP        1
#define SHI_ENABLE_UNTIL         1
#define SHI_ENABLE_VARIABLE      1
#define SHI_ENABLE_WHILE         1
#define SHI_ENABLE_XOR           1
#define SHI_ENABLE_BRACKET_LEFT  1
#define SHI_ENABLE_BRACKET_TICK  1
#define SHI_ENABLE_BRACKET_RIGHT 1
#define SHI_ENABLE_ZERO_NE       1
#define SHI_ENABLE_ZERO_MORE     1
#define SHI_ENABLE_TWO_TO_R      1
#define SHI_ENABLE_TWO_R_FROM    1
#define SHI_ENABLE_TWO_R_FETCH   1
#define SHI_ENABLE_COLON_NONAME  1
#define SHI_ENABLE_NE            1
#define SHI_ENABLE_Q_DO          1
#define SHI_ENABLE_ACTION_OF     1
#define SHI_ENABLE_AGAIN         1
#define SHI_ENABLE_BUFFER_COLON  1
#define SHI_ENABLE_CASE          1
#define SHI_ENABLE_COMPILE_COMMA 1
#define SHI_ENABLE_DEFER         1
#define SHI_ENABLE_DEFER_STORE   1
#define SHI_ENABLE_DEFER_FETCH   1
#define SHI_ENABLE_ENDCASE       1
#define SHI_ENABLE_ENDOF         1
#define SHI_ENABLE_ERASE         1
#define SHI_ENABLE_FALSE         1
#define SHI_ENABLE_HEX           1
#define SHI_ENABLE_IS            1
#define SHI_ENABLE_MARKER        1
#define SHI_ENABLE_NIP           1
#define SHI_ENABLE_OF            1
#define SHI_ENABLE_PAD           1
#define SHI_ENABLE_PARSE_NAME    1
#define SHI_ENABLE_PICK          1
#define SHI_ENABLE_ROLL          1
#define SHI_ENABLE_TRUE          1
#define SHI_ENABLE_TUCK          1
#define SHI_ENABLE_U_MORE        1
#define SHI_ENABLE_UNUSED        1
#define SHI_ENABLE_VALUE         1
#define SHI_ENABLE_WITHIN        1

// Shi words
#define SHI_ENABLE_TO_TEXT_Q     1
#define SHI_ENABLE_TO_DATA_Q     1
#define SHI_ENABLE_TO_TEXT       1
#define SHI_ENABLE_TO_DATA       1
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
extern uint32_t shi_context;

void shi_evaluate_asm(char const* str, size_t len);
void shi_variable_asm(char const* name, size_t len);
void shi_clear_asm();
void_fp shi_tick_asm(char const* str, size_t len);

#  ifdef __cplusplus
}
#  endif

// C only
#  ifndef __cplusplus

/// Add number to the top of the stack
///
/// \param  t   Number to push
static inline void shi_push_number(int32_t t) {
  asm volatile("ldr r0, [%0] \n"
               "str %1, [r0, #-4]! \n"
               "str r0, [%0] \n"
               :
               : "r"(&shi_context), "r"(t)
               : "memory", "r0");
}

/// Add double-cell number to the top of the stack
///
/// \param  t   Double-cell number to push
static inline void shi_push_double(int64_t t) {
  asm volatile("ldr r0, [%0] \n"
               "ldrd r1, r2, [%1] \n"
               "strd r2, r1, [r0, #-8]! \n"
               "str r0, [%0] \n"
               :
               : "r"(&shi_context), "r"(&t)
               : "memory", "r0", "r1", "r2");
}

/// Add user-defined type to the top of the stack
///
/// \param  t   Pointer to user-defined type
/// \param  n   Size of user-defined type
static inline void shi_push_struct(void* t, size_t n) {
  asm volatile("ldr r0, [%0] \n"

               // Insert padding if size isn't multiple of 4
               "mov r1, #0 \n"
               "str r1, [r0, #-4] \n"
               "and r1, %2, #3 \n"
               "sub r0, r0, r1 \n"

               // Do not change input operands
               "sub r1, r0, %2 \n"
               "add r2, %1, %2 \n"

               "1: cmp r0, r1 \n"
               "beq 1f \n"
               "ldrb r3, [r2, #-1]! \n"
               "strb r3, [r0, #-1]! \n"
               "b 1b \n"

               "1: str r0, [%0] \n"
               :
               : "r"(&shi_context), "r"(t), "r"(n)
               : "cc", "memory", "r0", "r1", "r2", "r3");
}

/// Remove number from the top of the stack
///
/// \return Number
static inline int32_t shi_pop_number() {
  int32_t t;

  asm volatile("ldr r0, [%1] \n"
               "ldr %0, [r0], #4 \n"
               "str r0, [%1] \n"
               : "=&r"(t)
               : "r"(&shi_context)
               : "memory", "r0");

  return t;
}

/// Remove double-cell number from the top of the stack
///
/// \return Double-cell number
static inline int64_t shi_pop_double() {
  int64_t t;

  asm volatile("ldr r0, [%0] \n"
               "ldrd r1, r2, [r0], #8 \n"
               "strd r2, r1, [%1] \n"
               "str r0, [%0] \n"
               :
               : "r"(&shi_context), "r"(&t)
               : "memory", "r0", "r1", "r2");

  return t;
}

/// Remove user-defined type from the top of the stack
///
/// \param  t   Pointer to user-defined type
/// \param  n   Size of user-defined type
static inline void shi_pop_struct(void* t, size_t n) {
  asm volatile("ldr r0, [%0] \n"

               // Do not change input operands
               "adds r1, r0, %2 \n"
               "movs r2, %1 \n"

               "1: cmp r0, r1 \n"
               "beq 1f \n"
               "ldrb r3, [r0], #1 \n"
               "strb r3, [r2], #1 \n"
               "b 1b \n"

               // Align
               "1: add r0, r0, #3 \n"
               "bic r0, r0, #3 \n"
               "str r0, [%0] \n"
               :
               : "r"(&shi_context), "r"(t), "r"(n)
               : "cc", "memory", "r0", "r1", "r2", "r3");
}

/// Get number from the stack
///
/// \param  offset  Offset in cells
/// \return Number
static inline int32_t shi_top_number(size_t offset) {
  int32_t t;

  asm volatile("ldr r0, [%1] \n"
               "ldr %0, [r0, %2, lsl #2] \n"
               : "=r"(t)
               : "r"(&shi_context), "r"(offset)
               : "memory", "r0");

  return t;
}

/// Get double-cell number from the stack
///
/// \param  offset  Offset in cells
/// \return Double-cell number
static inline int64_t shi_top_double(size_t offset) {
  int64_t t;

  asm volatile("ldr r0, [%0] \n"
               "add r0, r0, %2, lsl #2 \n"
               "ldrd r1, r2, [r0] \n"
               "strd r2, r1, [%1] \n"
               :
               : "r"(&shi_context), "r"(&t), "r"(offset)
               : "memory", "r0", "r1", "r2");

  return t;
}

/// Get user-defined type from the stack
///
/// \param  t       Pointer to user-defined type
/// \param  offset  Offset in cells
static inline void shi_top_struct(void** t, size_t offset) {
  asm volatile("ldr r0, [%1] \n"
               "add %0, r0, %2, lsl #2 \n"
               : "=r"(*t)
               : "r"(&shi_context), "r"(offset)
               : "memory", "r0");
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
static inline size_t shi_depth() {
  size_t size;

  asm volatile("ldr r0, [%1] \n"
               "sub %0, %1, r0 \n"
               "lsr %0, %0, #2 \n"
               "sub %0, #1 \n"
               : "=r"(size)
               : "r"(&shi_context)
               : "memory", "r0");

  return size;
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
static inline size_t shi_size() {
  return shi_depth();
}

/// Init struct
typedef struct {
  uint32_t data_begin;
  uint32_t data_end;
  uint32_t text_begin;
  uint32_t text_end;
  uint8_t text_p2align;
} Shi_init;

void shi_init_asm(Shi_init*);

/// Initialize
///
/// \param  s   Init struct
static inline void shi_init(Shi_init s) {
  shi_init_asm(&s);
}

/// Evaluate
///
/// \param  str Pointer to the null-terminated byte string
static inline void shi_evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
static inline void shi_evaluate_len(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

/// Create reference to C variable
///
/// \param  adr Address of variable
/// \param  str Pointer to the null-terminated byte string
static inline void shi_variable(void* adr, char const* str) {
  shi_push_number((int32_t)(adr));
  shi_variable_asm(str, strlen(str));
}

/// Create reference to C variable
///
/// \param  adr Address of variable
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
static inline void shi_variable_len(void* adr, char const* str, size_t len) {
  shi_push_number((int32_t)(adr));
  shi_variable_asm(str, len);
}

/// Clear stack
static inline void shi_clear() {
  shi_clear_asm();
}

/// Word
typedef struct {
  void_fp fp;
} Word;

/// Tick
///
/// \param  str Pointer to the null-terminated byte string
/// \return Word
static inline Word shi_tick(char const* str) {
  Word w = {shi_tick_asm(str, strlen(str))};
  return w;
}

/// Tick
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
/// \return Word
static inline Word shi_tick_len(char const* str, size_t len) {
  Word w = {shi_tick_asm(str, len)};
  return w;
}

/// Call word
///
/// \param  w   Word
static inline void shi_word(Word w) {
  if (w.fp)
    asm volatile("ldrd r7, r8, [%0] \n"
                 "ldr r6, [r7], #4 \n"
                 "push {%0} \n"
                 "blx %1 \n"
                 "pop {%0} \n"
                 "str r6, [r7, #-4]! \n"
                 "strd r7, r8, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(w.fp)
                 : "cc", "memory", "r6", "r7", "r8", "lr");
}

// C++ only
#  else

#    include <tuple>
#    include <type_traits>
#    include <functional>

namespace shi {

// Shouldn't this be part of C++20?
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
inline constexpr bool is_reference_wrapper_v{is_reference_wrapper<T>::value};

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
inline size_t depth() {
  size_t size;

  asm volatile("ldr r0, [%1] \n"
               "sub %0, %1, r0 \n"
               "lsr %0, %0, #2 \n"
               "sub %0, #1 \n"
               : "=r"(size)
               : "r"(&shi_context)
               : "memory", "r0");

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
void push(T&& t) {
  using std::addressof, std::is_arithmetic_v, std::is_pointer_v;
  using V = remove_cvref_t<T>;

  if constexpr (sizeof(V) <= 4 && (is_arithmetic_v<V> || is_pointer_v<V> ||
                                   is_reference_wrapper_v<V>))
    asm volatile("ldr r0, [%0] \n"
                 "str %1, [r0, #-4]! \n"
                 "str r0, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(t)
                 : "memory", "r0");
  else if constexpr (sizeof(V) == 8 && is_arithmetic_v<V>)
    asm volatile("ldr r0, [%0] \n"
                 "ldrd r1, r2, [%1] \n"
                 "strd r2, r1, [r0, #-8]! \n"
                 "str r0, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t))
                 : "memory", "r0", "r1", "r2");
  else {
    static_assert(sizeof(V) <= SHI_STACK_SIZE);
    asm volatile("ldr r0, [%0] \n"

                 // Insert padding if size isn't multiple of 4
                 "mov r1, #0 \n"
                 "str r1, [r0, #-4] \n"
                 "and r1, %2, #3 \n"
                 "sub r0, r0, r1 \n"

                 // Do not change input operands
                 "sub r1, r0, %2 \n"
                 "add r2, %1, %2 \n"

                 "1: cmp r0, r1 \n"
                 "beq 1f \n"
                 "ldrb r3, [r2, #-1]! \n"
                 "strb r3, [r0, #-1]! \n"
                 "b 1b \n"

                 "1: str r0, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t)), "r"(sizeof(V))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  }
}

/// Add elements to the top of the stack
///
/// \tparam Ts  Types of elements to push
/// \param  ts  Values to push
template<typename... Ts>
void push(Ts&&... ts) {
  using std::forward;

  static_assert((0 + ... + sizeof(remove_cvref_t<Ts>)) <= SHI_STACK_SIZE);

  (push(forward<Ts>(ts)), ...);
}

/// Remove element from the top of the stack
///
/// \tparam T   Type of element to pop
/// \return Value
template<typename T = int32_t>
T pop() {
  using std::addressof, std::is_arithmetic_v, std::is_pointer_v;

  T t;

  if constexpr (sizeof(T) <= 4 && (is_arithmetic_v<T> || is_pointer_v<T> ||
                                   is_reference_wrapper_v<T>))
    asm volatile("ldr r0, [%1] \n"
                 "ldr %0, [r0], #4 \n"
                 "str r0, [%1] \n"
                 : "=&r"(t)
                 : "r"(&shi_context)
                 : "memory", "r0");
  else if constexpr (sizeof(T) == 8 && is_arithmetic_v<T>)
    asm volatile("ldr r0, [%0] \n"
                 "ldrd r1, r2, [r0], #8 \n"
                 "strd r2, r1, [%1] \n"
                 "str r0, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t))
                 : "memory", "r0", "r1", "r2");
  else {
    static_assert(sizeof(T) <= SHI_STACK_SIZE);
    asm volatile("ldr r0, [%0] \n"

                 // Do not change input operands
                 "adds r1, r0, %2 \n"
                 "movs r2, %1 \n"

                 "1: cmp r0, r1 \n"
                 "beq 1f \n"
                 "ldrb r3, [r0], #1 \n"
                 "strb r3, [r2], #1 \n"
                 "b 1b \n"

                 // Align
                 "1: add r0, r0, #3 \n"
                 "bic r0, r0, #3 \n"
                 "str r0, [%0] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t)), "r"(sizeof(T))
                 : "cc", "memory", "r0", "r1", "r2", "r3");
  }

  return t;
}

/// Remove elements from the top of the stack
///
/// \tparam Ts  Types of element to pop
/// \return Values
template<typename... Ts, typename = std::enable_if_t<(sizeof...(Ts) > 1)>>
std::tuple<Ts...> pop() {
  using std::tuple;

  static_assert((0 + ... + sizeof(Ts)) <= SHI_STACK_SIZE);

  return tuple<Ts...>{pop<Ts>()...};
}

/// Get element from the stack
///
/// \tparam T       Type of element to get
/// \param  offset  Offset in cells
/// \return Value for built-in types
/// \return Reference for user-defined types
template<typename T = int32_t>
decltype(auto) top(size_t offset = 0) {
  using std::add_lvalue_reference_t, std::addressof, std::is_arithmetic_v,
      std::is_pointer_v;

  if constexpr (sizeof(T) <= 4 && (is_arithmetic_v<T> || is_pointer_v<T> ||
                                   is_reference_wrapper_v<T>)) {
    T t;
    asm volatile("ldr r0, [%1] \n"
                 "ldr %0, [r0, %2, lsl #2] \n"
                 : "=r"(t)
                 : "r"(&shi_context), "r"(offset)
                 : "memory", "r0");
    return t;
  } else if constexpr (sizeof(T) == 8 && is_arithmetic_v<T>) {
    T t;
    asm volatile("ldr r0, [%0] \n"
                 "add r0, r0, %2, lsl #2 \n"
                 "ldrd r1, r2, [r0] \n"
                 "strd r2, r1, [%1] \n"
                 :
                 : "r"(&shi_context), "r"(addressof(t)), "r"(offset)
                 : "memory", "r0", "r1", "r2");
    return t;
  } else {
    static_assert(sizeof(T) <= SHI_STACK_SIZE);
    T* t;
    asm volatile("ldr r0, [%1] \n"
                 "add %0, r0, %2, lsl #2 \n"
                 : "=r"(t)
                 : "r"(&shi_context), "r"(offset)
                 : "memory", "r0");
    return add_lvalue_reference_t<T>(*t);
  }
}

/// Init struct
struct Init {
  uint32_t data_begin{};
  uint32_t data_end{};
  uint32_t text_begin{};
  uint32_t text_end{};
  uint8_t text_p2align{2};
};

extern "C" void shi_init_asm(Init&);

/// Initialize
///
/// \param  s   Init struct
inline void init(Init s) {
  shi_init_asm(s);
}

/// Evaluate
///
/// \param  str Pointer to the null-terminated byte string
inline void evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
inline void evaluate(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

/// Create reference to C variable
///
/// \tparam T   Type of address
/// \param  adr Address of variable
/// \param  str Pointer to the null-terminated byte string
template<typename T>
void variable(T adr, char const* str) {
  using std::is_pointer_v;

  static_assert(sizeof(T) == 4 &&
                (is_pointer_v<T> || is_reference_wrapper_v<T>));

  push(adr);
  shi_variable_asm(str, strlen(str));
}

/// Create reference to C variable
///
/// \tparam T   Type of address
/// \param  adr Address of variable
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
template<typename T>
void variable(T adr, char const* str, size_t len) {
  using std::is_pointer_v;

  static_assert(sizeof(T) == 4 &&
                (is_pointer_v<T> || is_reference_wrapper_v<T>));

  push(adr);
  shi_variable_asm(str, len);
}

/// Clear stack
inline void clear() {
  shi_clear_asm();
}

/// Word
struct Word {
  constexpr Word() = default;
  Word(char const* str) : fp{shi_tick_asm(str, strlen(str))} {}
  Word(char const* str, size_t len) : fp{shi_tick_asm(str, len)} {}

  /// Call word
  ///
  /// \tparam   Ts  Types of arguments
  /// \param    ts  Arguments
  template<typename... Ts>
  Word& operator()(Ts&&... ts) {
    using std::forward;

    if (fp) {
      push(forward<Ts>(ts)...);

      asm volatile("ldrd r7, r8, [%0] \n"
                   "ldr r6, [r7], #4 \n"
                   "push {%0} \n"
                   "blx %1 \n"
                   "pop {%0} \n"
                   "str r6, [r7, #-4]! \n"
                   "strd r7, r8, [%0] \n"
                   :
                   : "r"(&shi_context), "r"(fp)
                   : "cc", "memory", "r6", "r7", "r8", "lr");
    }

    return *this;
  }

  /// Implicit conversion return
  template<typename T>
  operator T() {
    return pop<T>();
  }

  /// Implicit conversion multiple returns
  template<typename... Ts>
  operator std::tuple<Ts...>() {
    return pop<Ts...>();
  }

private:
  void_fp fp{nullptr};
};

namespace literals {

/// Literal evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
inline void operator"" _s(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

/// Literal variable
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
/// \return Callable to be invoked with address
inline auto operator""_v(char const* str, size_t len) {
  return [=](auto adr) {
    using std::is_pointer_v;

    static_assert(sizeof(adr) == 4 && (is_pointer_v<decltype(adr)> ||
                                       is_reference_wrapper_v<decltype(adr)>));

    push(adr);
    shi_variable_asm(str, len);
  };
}

/// Literal word
///
/// \tparam T   Type of character sequence
/// \tparam Cs  Character sequence
/// \return Word
template<typename T, T... Cs>
Word operator""_w() {
  static constexpr char c[]{Cs...};
  static Word word{c, sizeof...(Cs)};
  return word;
}

}  // namespace literals

}  // namespace shi

#  endif

#endif
