/// Doc
///
/// \file   doc.hpp
/// \author Vincent Hamp
/// \date   09/04/2019

/// \mainpage Shi
/// Shi is an unusual Forth in a sense that it's not a full-fledged
/// self-contained system. Neither
/// [key](http://forth-standard.org/standard/core/KEY)/[emit](http://forth-standard.org/standard/core/EMIT),
/// which deal with user input, nor any other input or string conversion words
/// (e.g.
/// [#](https://forth-standard.org/standard/core/num),
/// [accept](https://forth-standard.org/standard/core/ACCEPT), ...) are
/// implemented. Instead of a
/// [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop)
/// Shi is intended to be used as embedded scripting language by featuring an
/// extensive C/C++ API. The functions shi::evaluate and \ref shi_evaluate can
/// be used to enter the classic interpretation loop. This reflects what the
/// word [evaluate](https://forth-standard.org/standard/core/EVALUATE) normally
/// does.
///
/// \dot
/// digraph G {
///
///   subgraph cluster_c_cpp {
///     label = "C/C++";
///     labeljust="l";
///     style="dotted";
///     shi_evaluate;
///   }
///
///   subgraph cluster_forth {
///     label = "Shi";
///     labeljust="l";
///     style="dotted";
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
/// - The implementation of data-, control- and return-stack
/// - Dictionary entry layouts and word flags
/// - Relationship between code- and data space
/// - Action on an ambiguous condition
///
/// and of course to non-standard additions such as
/// - Compilation to flash by offering two different data-spaces (data/text ==
///   ram/flash)
/// - Optimizations such as inlining and constant folding
///
// clang-format off
/// \page page_implementation Implementation
/// | Page                              | Content                                           |
/// | --------------------------------- | ------------------------------------------------- |
/// | \subpage page_stacks              | Handling of data-, control-flow- and return-stack |
/// | \subpage page_variables           | Internally used variables                         |
/// | \subpage page_dictionary          | Layout of words, core- and user dictionary        |
/// | \subpage page_extensions          | Extensions                                        |
/// | \subpage page_wordlist            | Wordlist                                          |
/// | \subpage page_ambiguous_condition | Ambiguous conditions                              |
/// | \subpage page_interpret           | Interpretation loop in detail                     |
/// | \subpage page_control_structures  | Control structures                                |
/// | \subpage page_todo                | TODO list                                         |
// clang-format on
///
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
/// new definition. The way the standard phrases this is that *interpretation
/// semantics are undefined*.
///
/// So this works
///
/// ```asm
/// : lunch? if eat then ;
/// ```
///
/// whereas this doesn't.
///
/// ```asm
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
/// ```asm
/// tos .req r6                             @ Top of stack
/// dsp .req r7                             @ Data-stack pointer
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
/// \note All stacks are 32bit wide which is the native word size on ARMv7-M.
///
///
/// \page page_variables Variables
/// There are a couple of states Shi needs to track internally. When evaluating
/// the total ram usage these variables have to be taken into consideration and
/// added to the user-defined stack size. The table below contains them all
/// including a short description and their size in bytes.
///
// clang-format off
/// \page page_variables Variables
/// | Symbol          | Description                                                                        | Size [b]        |
/// | ----------------| ---------------------------------------------------------------------------------- | --------------- |
/// | shi_stack_begin | Stack begin                                                                        | user-defined + 4|
/// | shi_stack_end   | Stack end                                                                          |                 |
/// | shi_context     | Context (dsp and lfp)                                                              | 8               |
/// |                 |                                                                                    |                 |
/// | to_text_begin   | Pointer to current ram begin which is going to end in flash                        | 4               |
/// | data_begin      | Pointer to ram begin                                                               | 4               |
/// | data_end        | Pointer to ram end, used for reserving ram for variables                           | 4               |
/// | text_begin      | Pointer to flash begin                                                             | 4               |
/// | text_end        | Pointer to flash end                                                               | 4               |
/// |                 |                                                                                    |                 |
/// | csp             | Inside loop: points to leave addresses from the current loop on the stack <br><!-- |                 |
/// |                 | --> Inside case: points to endof addresses from the current case on the stack      | 4               |
/// | link            | Contains address of link of the last definition in ram                             | 4               |
/// | status          | Current state (state is taken as word) <br><!--                                    |                 |
/// |                 | --> false: interpret, true: compile                                                | 4               |
/// | src             | Source (address and length)                                                        | 8               |
/// | in              | Index in terminal input buffer                                                     | 4               |
/// | radix           | Determine current numerical base (base is taken as word)                           | 4               |
/// | text_align      | User defined alignment for flash                                                   | 1               |
/// | leave_lvl       | Current nesting level of do...loop                                                 | 1               |
/// | case_lvl        | Current nesting level of case...of...endof...endcase                               | 1               |
// clang-format on
///
/// Most variables should be more or less evident with some exceptions:
///
/// \page page_variables Variables
/// **shi_context**<br>
/// Shi maps the top-of-stack, the data-space-pointer and the
/// literal-folding-pointer directly to registers. Those registers must be
/// restored and saved upon entry and exit. The top-of-stack gets pushed or
/// popped to the internal data-stack and the pointers get stored in an own
/// variable called shi_context.
///
/// **data_begin, data_end, text_begin, text_end**<br>
/// Usually Forth system have a single continuous data-space but Shi supports
/// compilation to two different data-spaces (data and text aka ram and flash).
/// The variables data_begin, data_end, text_begin and text_end keep track of
/// the addresses of the data-spaces passed in by the user at initialization.
/// Writing to data or text advances those data-space pointers.
///
/// **to_text_begin**<br>
/// Shi does not write directly to flash but first stores new definitions in ram
/// before a special word calls an extern function which then takes care of
/// copying the definitions over. to_text_begin marks the beginning of the
/// memory which should be written into flash.
///
/// **text_align**<br>
/// Most flash memories have alignment restrictions and can only write to 4-, 8-
/// or even 16-byte aligned addresses. This alignment requirements are stored as
/// power of 2 in text_align and can be set by the user at initialization.
///
/// **leave_lvl, case_lvl**<br>
/// The control-flow words
/// [endof](https://forth-standard.org/standard/core/ENDOF) and
/// [leave](https://forth-standard.org/standard/core/LEAVE) can nest an
/// arbitrary number of times and even mix with other control-flow words. To
/// allow that Shi has to internally track the current nesting level of loop-
/// and case-sys.
///
///
/// \page page_dictionary Dictionary
/// All definitions in Forth must share a certain header which contains
/// - a link to the previous or next definition
/// - some flags which tell the interpreter how to treat the word and
/// - a name for actual look-up
///
// clang-format off
/// \page page_dictionary Dictionary
/// | Field      | Description                                      | Size [b]                |
/// | -----------| ------------------------------------------------ | ----------------------- |
/// | Link       | Link to previous or next definition              | 4                       |
/// | Flags      | Properties of word (e.g. immediate, inline, ...) | 1                       |
/// | Name       | Counted string                                   | 1 length + length chars |
/// | Code/data  | Code or data                                     | user-defined            |
// clang-format on
/// \page page_dictionary Dictionary
///
/// Shi is what's called a direct threaded Forth which means that the execution
/// token of a word equals it's very first assembly instruction. Since the
/// ARMv7-M architecture has certain alignment restrictions and can only execute
/// code from 2-byte aligned addresses there might be a padding byte right after
/// the definitions name.
///
/// **Dictionary types**<br>
/// Although in practice there is just a single type of dictionary entry it's
/// useful to differ between the data-space a definition resides in and whether
/// it's part of the core dictionary or not. Using this properties we can divide
/// Shi's dictionary into:
/// -# Core dictionary - part of Shi itself
/// -# User dictionary in data - extended through the user and compiled into
///    data
/// -# User dictionary in text - extended through the user and compiled into
///    text
///
/// **Creating the core dictionary**<br>
/// The core dictionary is created by some macro magic heavily inspired by
/// [Mecrisp-Stellaris](http://mecrisp.sourceforge.net/>Mecrisp-Stellaris). The
/// macro *WORD* can be used to automatically create a linked list of assembler
/// functions including flags and a counted string name. The macro parameter
/// *label* is optional and only necessary if the name of the word contains
/// special characters which are not allowed as assembly labels, otherwise
/// *name* is also used as label. Since *WORD* uses the numeric labels 7, 8 and
/// 9 the actual definition may only use 1-6 for its own branches.
///
// clang-format off
/// \page page_dictionary Dictionary
/// ```asm
/// .macro WORD flags, name, label
///     .p2align 1                          @ Align before link
/// link\@\():                              @ Label the link
/// 9:  .word 9f                            @ Link (4 byte)
///     .byte \flags                        @ Flags (1 byte)
///     .byte 8f - 7f                       @ Length (1 byte)
/// 7:  .ascii "\name"                      @ Name (cstring)
/// 8:  .p2align 1                          @ Align before code
/// .thumb_func
/// .ifnb \label                            @ Label for code (use name if label wasn't defined)
/// \label\():
/// .else
/// \name\():
/// .endif
/// .endm
/// ```
// clang-format on
/// \page page_dictionary Dictionary
///
/// Here is an example of the definition of
/// [+](https://forth-standard.org/standard/core/Plus) created with *WORD*.
///
/// ```asm
/// WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "+", plus
///     ldmia dsp!, {r0}
///     adds tos, r0
///     bx lr
/// ```
///
/// **Flags**<br>
/// The standard differs between three different properties a word can have. It
/// might have interpretation semantics, compilation semantics and it might be
/// immediate. In theory any combination of those three can occur although some
/// like interpretation semantics and immediate might not make much sense. Shi
/// comes with additional flags for its optimizations and feature to compile to
/// flash. Specially the latter is tricky since
/// [variable](https://forth-standard.org/standard/core/VARIABLE)s compiled to
/// it still need to have a cell of ram memory somewhere. For that reason the
/// definition gets marked with the *RESERVE_x* flag which lets Shi reserve
/// memory cells at the end of data-space at initialization.
///
// clang-format off
/// \page page_dictionary Dictionary
/// | Flag           | Description                                                      | Value       |
/// | ---------------| ---------------------------------------------------------------- | ----------- |
/// | FLAG_SKIP      | Definition ignored                                               | 0b1111'1111 |
/// | FLAG_INTERPRET | Definition has interpretation semantics                          | 0b0111'1111 |
/// | FLAG_COMPILE   | Definition has compilation semantics                             | 0b1011'1111 |
/// | FLAG_IMMEDIATE | Definition is immediate (and executes during compilation)        | 0b1101'1111 |
/// | FLAG_INLINE    | Definition is short enough to get inlined instead of called      | 0b1110'1111 |
/// | RESERVE_x      | Definition needs to reserve cells of data-space                  | 0b1111'xx11 |
/// | FOLDS_x        | Definition allows constant folding (e.g. 3 4 + is replaced by 7) | 0b1111'11xx |
// clang-format on
/// \page page_dictionary Dictionary
///
/// **Search order**<br>
/// As mentioned at page \ref page_variables the link to the latest definition
/// in ram is always stored. In case there is no definition in ram yet it still
/// has its initial value which is the start of the core dictionary. Anyhow link
/// provides the start of a singly linked list which iterates through the
/// dictionary in the following order:
/// -# User dictionary in data
/// -# Core dictionary in text
/// -# User dictionary in text
///
/// In case the user hasn't extended the dictionary so far it looks like this:
///
/// \dot
/// digraph G {
///
///   newrank=true;
///   rankdir=LR;
///
///   subgraph cluster_data {
///     label="data";
///     labeljust="l";
///     style=dotted;
///
///     subgraph cluster_data_core {
///       label="core";
///       labeljust="l";
///       style=solid;
///       color="#FBB4AE";
///       d0[style=invis];
///       d1[style=invis];
///       d2[style=invis];
///       d3[style=invis];
///       d4[style=filled, label=""];
///     }
///   }
///
///   subgraph cluster_text {
///     label="text";
///     labeljust="l";
///     style=dotted;
///
///     subgraph cluster_text_core {
///       label="core";
///       labeljust="l";
///       style=solid;
///       color="#FBB4AE";
///       t0[label="!"];
///       t1[label="'"];
///       t2[shape=plaintext, label="..."];
///       t3[label="u>"];
///       t4[style=invis];
///     }
///   }
///
///   link->t0->t1->t2->t3->d4;
///
///   {rank=same d0 t0}
///   {rank=same d1 t1}
///   {rank=same d2 t2}
///   {rank=same d3 t3}
///   {rank=same d4 t4}
/// }
/// \enddot
///
/// The light gray entry is special because it's the very last entry of the core
/// dictionary. It is also the only definition of the core which resides in data
/// and not in text. This is a necessity to allow the very last link of the core
/// dictionary to point to the first entry of the user dictionary in text
/// without recompiling Shi. The address of the user dictionary is simply not
/// known until runtime when it's passed as parameter to the initialization
/// function.
///
/// Once the user starts adding definitions in both data-spaces the dictionary
/// might change it's appearance to something like this:
///
/// \dot
/// digraph G {
///
///   newrank=true;
///   rankdir=LR;
///
///   subgraph cluster_data {
///     label="data";
///     labeljust="l";
///     style=dotted;
///
///     subgraph cluster_data_core {
///       label="core";
///       labeljust="l";
///       style=solid;
///       color="#FBB4AE";
///       d4[style=invis];
///       d5[style=invis];
///       d6[style=invis];
///       d7[style=filled, label=""];
///       d8[style=invis];
///       d9[style=invis];
///     }
///
///     subgraph cluster_data_user {
///       label="user";
///       labeljust="l";
///       style=solid;
///       color="#B3CDE3"
///       d0[label=""];
///       d1[label=""];
///       d2[label=""];
///       d3[style=invis];
///     }
///   }
///
///   subgraph cluster_text {
///     label="text";
///     labeljust="l";
///     style=dotted;
///
///     subgraph cluster_text_core {
///       label="core";
///       labeljust="l";
///       style=solid;
///       color="#FBB4AE";
///       t0[style=invis];
///       t1[style=invis];
///       t2[style=invis];
///       t3[label="!"];
///       t4[label="'"];
///       t5[shape=plaintext, label="..."];
///       t6[label="u>"];
///       t7[style=invis];
///     }
///
///     subgraph cluster_text_user {
///       label="user";
///       labeljust="l";
///       style=solid;
///       color="#B3CDE3"
///       t8[label=""];
///       t9[label=""];
///     }
///   }
///
///   link->d0->d1->d2->t3->t4->t5->t6->d7->t8->t9;
///
///   {rank=same d0 t0}
///   {rank=same d1 t1}
///   {rank=same d2 t2}
///   {rank=same d3 t3}
///   {rank=same d4 t4}
///   {rank=same d5 t5}
///   {rank=same d6 t6}
///   {rank=same d7 t7}
///   {rank=same d8 t8}
///   {rank=same d9 t9}
/// }
/// \enddot
///
/// An implication of this search order is that definitions in data are found
/// faster than those in text.
///
/// **Initialization**<br>
/// To initialize Shi the functions shi::init and \ref shi_init can be used.
/// Both functions take a struct which contains the begin and end addresses of
/// the data-spaces as well as the necessary text alignment for compilation to
/// flash. Passing addresses and alignment for text is completely optional and
/// can simply be set to 0 if not needed.
///
/// Besides applying the passed addresses there are three things happening
/// during initialization.
///
/// -# Sweep text<br>
///    The whole dictionary is searched for definitions which need to reserve
///    ram. The necessary amount of ram memory is taken from the end of
///    data-space. Afterwards the last found link is saved as beginning of the
///    text data-space. At this point the last link might either be the last
///    core dictionary entry or the last user dictionary entry depending on
///    whether the user has already extended the dictionary or not.
///
/// -# Fill data<br>
///    Shi does not rely on whether the data-space passed in is zero-initialized
///    or not. In any case it gets overwritten by the value defined by \ref
///    SHI_ERASED_WORD. By default that's 0xFFFFFFFF which mimics what most
///    cleared flash devices are.
///
/// -# Set context<br>
///    Initializing the Shi context which means initializing the following
///    registers
///    ```asm
///    tos .req r6                             @ Top of stack
///    dsp .req r7                             @ Data-stack pointer
///    lfp .req r8                             @ Literal-folding pointer
///    ```
///    *tos* gets initialized with '*', *dsp* with the stack end and *lfp* with
///    0.
///
///
/// \page page_control_structures Control structures
/// TODO
///
///
/// \page page_extensions Extensions
/// Shi adds a couple of non-ANS words to Forth.
///
/// **h!**<br>
/// Like [!](https://forth-standard.org/standard/core/Store) but stores a
/// halfword (16bit value).
///
/// ```cpp
/// "42 var h!"_s;
/// ```
///
/// <b>h@@</b><br>
/// Like [@@](https://forth-standard.org/standard/core/Fetch) but fetches a
/// halfword (16bit value).
///
/// ```cpp
/// "var h@"_s;
/// ```
///
/// **inline**<br>
/// Make the most recent definition an inline word.
///
/// ```cpp
/// ": add + ; inline"_s;
/// ```
///
/// <b>>text?, >data?</b><br>
/// text? and data? can be used to check if Shi is currently compiling to ram or
/// flash memory.
///
/// ```cpp
/// "data?"_s;  // ( -- true | false )
/// "text?"_s;  // ( -- true | false )
/// ```
///
/// <b>>text, >data</b><br>
/// By default (and after initialization) Shi compiles to ram (data). It is
/// possible to compile definitions to flash (text) by wrapping them in a
/// >text ... >data block.
///
/// ```cpp
/// // All following definitions get compiled to text
/// ">text"_s;
/// ": six 6 ;"_s;
/// ": seven six 1+ ;"_s;
/// ">data"_s;
/// ```
///
/// The actual flash write takes place once the block ends with >data. Until
/// then everything between >text and >data is stored in data. Implementing the
/// flash write is the users responsibility. Shi anticipates a callback with the
/// symbol \ref shi_write_text which takes 3 arguments:
/// -# Begin of block to write
/// -# End of block to write
/// -# Begin of text area to write to
///
/// A reference implementation for the STM32 F4-Discovery board can be found in
/// the ./src/test folder. After the call to \ref shi_write_text data gets
/// cleared till the beginning of the current block.
///
/// \warning Make sure you passed the right text-alignment during
/// initialization. E.g., flash memory on the STM32 F4-Discovery board can only
/// be written in double-words, so the init call must adjust .text_p2align like
/// this:
/// ```cpp
/// shi::init({.data_begin = (uint32_t)(begin(data)),
///            .data_end = (uint32_t)(end(data)),
///            .text_begin = FLASH_END - 32 * 1024,
///            .text_end = FLASH_END,
///            .text_p2align = 3});
/// ```
///
///
// clang-format off
/// \page page_wordlist Wordlist
/// | Core word set                                                           | Description                                           |
/// | ----------------------------------------------------------------------- | ----------------------------------------------------- |
/// | [!](https://forth-standard.org/standard/core/Store)                     | exactly ANS                                           |
/// | [#](https://forth-standard.org/standard/core/num)                       | unimplemented                                         |
/// | [#&gt;](https://forth-standard.org/standard/core/num-end)               | unimplemented                                         |
/// | [#s](https://forth-standard.org/standard/core/numS)                     | unimplemented                                         |
/// | ['](https://forth-standard.org/standard/core/Tick)                      | exactly ANS                                           |
/// | [(](https://forth-standard.org/standard/core/p)                         | unimplemented                                         |
/// | [*](https://forth-standard.org/standard/core/Times)                     | exactly ANS                                           |
/// | [*&frasl;](https://forth-standard.org/standard/core/TimesDiv)           | exactly ANS                                           |
/// | [*&frasl;mod](https://forth-standard.org/standard/core/TimesDivMOD)     | TBD                                                   |
/// | [+](https://forth-standard.org/standard/core/Plus)                      | exactly ANS                                           |
/// | [+!](https://forth-standard.org/standard/core/PlusStore)                | exactly ANS                                           |
/// | [+loop](https://forth-standard.org/standard/core/PlusLOOP)              | exactly ANS                                           |
/// | [,](https://forth-standard.org/standard/core/Comma)                     | exactly ANS                                           |
/// | [-](https://forth-standard.org/standard/core/Minus)                     | exactly ANS                                           |
/// | [.](https://forth-standard.org/standard/core/d)                         | unimplemented                                         |
/// | [.&quot;](https://forth-standard.org/standard/core/Dotq)                | unimplemented                                         |
/// | [&frasl;](https://forth-standard.org/standard/core/Div)                 | exactly ANS                                           |
/// | [&frasl;mod](https://forth-standard.org/standard/core/DivMOD)           | TBD                                                   |
/// | [0&lt;](https://forth-standard.org/standard/core/Zeroless)              | exactly ANS                                           |
/// | [0=](https://forth-standard.org/standard/core/ZeroEqual)                | exactly ANS                                           |
/// | [1+](https://forth-standard.org/standard/core/OnePlus)                  | exactly ANS                                           |
/// | [1-](https://forth-standard.org/standard/core/OneMinus)                 | exactly ANS                                           |
/// | [2!](https://forth-standard.org/standard/core/TwoStore)                 | exactly ANS                                           |
/// | [2*](https://forth-standard.org/standard/core/TwoTimes)                 | exactly ANS                                           |
/// | [2&frasl;](https://forth-standard.org/standard/core/TwoDiv)             | exactly ANS                                           |
/// | [2@@](https://forth-standard.org/standard/core/TwoFetch)                | exactly ANS                                           |
/// | [2drop](https://forth-standard.org/standard/core/TwoDROP)               | exactly ANS                                           |
/// | [2dup](https://forth-standard.org/standard/core/TwoDUP)                 | exactly ANS                                           |
/// | [2over](https://forth-standard.org/standard/core/TwoOVER)               | exactly ANS                                           |
/// | [2swap](https://forth-standard.org/standard/core/TwoSWAP)               | exactly ANS                                           |
/// | [:](https://forth-standard.org/standard/core/Colon)                     | exactly ANS                                           |
/// | [;](https://forth-standard.org/standard/core/Semi)                      | exactly ANS                                           |
/// | [&lt;](https://forth-standard.org/standard/core/less)                   | exactly ANS                                           |
/// | [&lt;#](https://forth-standard.org/standard/core/num-start)             | unimplemented                                         |
/// | [=](https://forth-standard.org/standard/core/Equal)                     | exactly ANS                                           |
/// | [&gt;](https://forth-standard.org/standard/core/more)                   | exactly ANS                                           |
/// | [&gt;body](https://forth-standard.org/standard/core/toBODY)             | exactly ANS                                           |
/// | [&gt;in](https://forth-standard.org/standard/core/toIN)                 | exactly ANS                                           |
/// | [&gt;number](https://forth-standard.org/standard/core/toNUMBER)         | unimplemented                                         |
/// | [&gt;r](https://forth-standard.org/standard/core/toR)                   | exactly ANS                                           |
/// | [?dup](https://forth-standard.org/standard/core/qDUP)                   | exactly ANS                                           |
/// | [@@](https://forth-standard.org/standard/core/Fetch)                    | exactly ANS                                           |
/// | [abort](https://forth-standard.org/standard/core/ABORT)                 | unimplemented                                         |
/// | [abort&quot;](https://forth-standard.org/standard/core/ABORTq)          | unimplemented                                         |
/// | [abs](https://forth-standard.org/standard/core/ABS)                     | exactly ANS                                           |
/// | [accept](https://forth-standard.org/standard/core/ACCEPT)               | unimplemented                                         |
/// | [align](https://forth-standard.org/standard/core/ALIGN)                 | exactly ANS                                           |
/// | [aligned](https://forth-standard.org/standard/core/ALIGNED)             | exactly ANS                                           |
/// | [allot](https://forth-standard.org/standard/core/ALLOT)                 | exactly ANS                                           |
/// | [and](https://forth-standard.org/standard/core/AND)                     | exactly ANS                                           |
/// | [base](https://forth-standard.org/standard/core/BASE)                   | exactly ANS                                           |
/// | [begin](https://forth-standard.org/standard/core/BEGIN)                 | exactly ANS                                           |
/// | [bl](https://forth-standard.org/standard/core/BL)                       | unimplemented                                         |
/// | [c!](https://forth-standard.org/standard/core/CStore)                   | exactly ANS                                           |
/// | [c,](https://forth-standard.org/standard/core/CComma)                   | exactly ANS                                           |
/// | [c@@](https://forth-standard.org/standard/core/CFetch)                  | exactly ANS                                           |
/// | [cell+](https://forth-standard.org/standard/core/CELLPlus)              | exactly ANS                                           |
/// | [cells](https://forth-standard.org/standard/core/CELLS)                 | exactly ANS                                           |
/// | [char](https://forth-standard.org/standard/core/CHAR)                   | unimplemented                                         |
/// | [char+](https://forth-standard.org/standard/core/CHARPlus)              | exactly ANS                                           |
/// | [chars](https://forth-standard.org/standard/core/CHARS)                 | exactly ANS                                           |
/// | [constant](https://forth-standard.org/standard/core/CONSTANT)           | exactly ANS                                           |
/// | [count](https://forth-standard.org/standard/core/COUNT)                 | unimplemented                                         |
/// | [cr](https://forth-standard.org/standard/core/CR)                       | unimplemented                                         |
/// | [create](https://forth-standard.org/standard/core/CREATE)               | exactly ANS                                           |
/// | [decimal](https://forth-standard.org/standard/core/DECIMAL)             | exactly ANS                                           |
/// | [depth](https://forth-standard.org/standard/core/DEPTH)                 | exactly ANS                                           |
/// | [do](https://forth-standard.org/standard/core/DO)                       | exactly ANS                                           |
/// | [does&gt;](https://forth-standard.org/standard/core/DOES)               | exactly ANS                                           |
/// | [drop](https://forth-standard.org/standard/core/DROP)                   | exactly ANS                                           |
/// | [dup](https://forth-standard.org/standard/core/DUP)                     | exactly ANS                                           |
/// | [else](https://forth-standard.org/standard/core/ELSE)                   | exactly ANS                                           |
/// | [emit](https://forth-standard.org/standard/core/EMIT)                   | unimplemented                                         |
/// | [environment?](https://forth-standard.org/standard/core/ENVIRONMENTq)   | unimplemented                                         |
/// | [evaluate](https://forth-standard.org/standard/core/EVALUATE)           | unimplemented                                         |
/// | [execute](https://forth-standard.org/standard/core/EXECUTE)             | exactly ANS                                           |
/// | [exit](https://forth-standard.org/standard/core/EXIT)                   | exactly ANS                                           |
/// | [fill](https://forth-standard.org/standard/core/FILL)                   | unimplemented                                         |
/// | [find](https://forth-standard.org/standard/core/FIND)                   | not ANS ( c-addr -- c-addr 0 \| xt flags )            |
/// | [fm&frasl;mod](https://forth-standard.org/standard/core/FMDivMOD)       | TBD                                                   |
/// | [here](https://forth-standard.org/standard/core/HERE)                   | exactly ANS                                           |
/// | [hold](https://forth-standard.org/standard/core/HOLD)                   | unimplemented                                         |
/// | [i](https://forth-standard.org/standard/core/I)                         | exactly ANS                                           |
/// | [if](https://forth-standard.org/standard/core/IF)                       | exactly ANS                                           |
/// | [immediate](https://forth-standard.org/standard/core/IMMEDIATE)         | exactly ANS                                           |
/// | [invert](https://forth-standard.org/standard/core/INVERT)               | exactly ANS                                           |
/// | [j](https://forth-standard.org/standard/core/J)                         | exactly ANS                                           |
/// | [key](https://forth-standard.org/standard/core/KEY)                     | unimplemented                                         |
/// | [leave](https://forth-standard.org/standard/core/LEAVE)                 | exactly ANS                                           |
/// | [literal](https://forth-standard.org/standard/core/LITERAL)             | exactly ANS                                           |
/// | [loop](https://forth-standard.org/standard/core/LOOP)                   | exactly ANS                                           |
/// | [lshift](https://forth-standard.org/standard/core/LSHIFT)               | exactly ANS                                           |
/// | [m*](https://forth-standard.org/standard/core/MTimes)                   | TBD                                                   |
/// | [max](https://forth-standard.org/standard/core/MAX)                     | exactly ANS                                           |
/// | [min](https://forth-standard.org/standard/core/MIN)                     | exactly ANS                                           |
/// | [mod](https://forth-standard.org/standard/core/MOD)                     | exactly ANS                                           |
/// | [move](https://forth-standard.org/standard/core/MOVE)                   | TBD                                                   |
/// | [negate](https://forth-standard.org/standard/core/NEGATE)               | exactly ANS                                           |
/// | [or](https://forth-standard.org/standard/core/OR)                       | exactly ANS                                           |
/// | [over](https://forth-standard.org/standard/core/OVER)                   | exactly ANS                                           |
/// | [postpone](https://forth-standard.org/standard/core/POSTPONE)           | exactly ANS                                           |
/// | [quit](https://forth-standard.org/standard/core/QUIT)                   | unimplemented                                         |
/// | [r&gt;](https://forth-standard.org/standard/core/Rfrom)                 | exactly ANS                                           |
/// | [r@@](https://forth-standard.org/standard/core/RFetch)                  | exactly ANS                                           |
/// | [recurse](https://forth-standard.org/standard/core/RECURSE)             | exactly ANS                                           |
/// | [repeat](https://forth-standard.org/standard/core/REPEAT)               | exactly ANS                                           |
/// | [rot](https://forth-standard.org/standard/core/ROT)                     | exactly ANS                                           |
/// | [rshift](https://forth-standard.org/standard/core/RSHIFT)               | exactly ANS                                           |
/// | [s&quot;](https://forth-standard.org/standard/core/Sq)                  | unimplemented                                         |
/// | [s&gt;d](https://forth-standard.org/standard/core/StoD)                 | TBD                                                   |
/// | [sign](https://forth-standard.org/standard/core/SIGN)                   | TBD                                                   |
/// | [sm&frasl;rem](https://forth-standard.org/standard/core/SMDivREM)       | TBD                                                   |
/// | [source](https://forth-standard.org/standard/core/SOURCE)               | exactly ANS                                           |
/// | [space](https://forth-standard.org/standard/core/SPACE)                 | unimplemented                                         |
/// | [spaces](https://forth-standard.org/standard/core/SPACES)               | unimplemented                                         |
/// | [state](https://forth-standard.org/standard/core/STATE)                 | exactly ANS                                           |
/// | [swap](https://forth-standard.org/standard/core/SWAP)                   | exactly ANS                                           |
/// | [then](https://forth-standard.org/standard/core/THEN)                   | exactly ANS                                           |
/// | [type](https://forth-standard.org/standard/core/TYPE)                   | unimplemented                                         |
/// | [u.](https://forth-standard.org/standard/core/Ud)                       | unimplemented                                         |
/// | [u&lt;](https://forth-standard.org/standard/core/Uless)                 | exactly ANS                                           |
/// | [um*](https://forth-standard.org/standard/core/UMTimes)                 | TBD                                                   |
/// | [um&frasl;mod](https://forth-standard.org/standard/core/UMDivMOD)       | TBD                                                   |
/// | [unloop](https://forth-standard.org/standard/core/UNLOOP)               | exactly ANS                                           |
/// | [until](https://forth-standard.org/standard/core/UNTIL)                 | exactly ANS                                           |
/// | [variable](https://forth-standard.org/standard/core/VARIABLE)           | exactly ANS                                           |
/// | [while](https://forth-standard.org/standard/core/WHILE)                 | exactly ANS                                           |
/// | [word](https://forth-standard.org/standard/core/WORD)                   | obsolete                                              |
/// | [xor](https://forth-standard.org/standard/core/XOR)                     | exactly ANS                                           |
/// | [\[](https://forth-standard.org/standard/core/Bracket)                  | exactly ANS                                           |
/// | [\['\]](https://forth-standard.org/standard/core/BracketTick)           | exactly ANS                                           |
/// | [\[char\]](https://forth-standard.org/standard/core/BracketCHAR)        | unimplemented                                         |
/// | [\]](https://forth-standard.org/standard/core/right-bracket)            | exactly ANS                                           |
/// | [.(](https://forth-standard.org/standard/core/Dotp)                     | unimplemented                                         |
/// | [.r](https://forth-standard.org/standard/core/DotR)                     | unimplemented                                         |
/// | [0&lt;&gt;](https://forth-standard.org/standard/core/Zerone)            | exactly ANS                                           |
/// | [0&gt;](https://forth-standard.org/standard/core/Zeromore)              | exactly ANS                                           |
/// | [2&gt;r](https://forth-standard.org/standard/core/TwotoR)               | exactly ANS                                           |
/// | [2r&gt;](https://forth-standard.org/standard/core/TwoRfrom)             | exactly ANS                                           |
/// | [2r@@](https://forth-standard.org/standard/core/TwoRFetch)              | exactly ANS                                           |
/// | [:noname](https://forth-standard.org/standard/core/ColonNONAME)         | unimplemented                                         |
/// | [&lt;&gt;](https://forth-standard.org/standard/core/ne)                 | exactly ANS                                           |
/// | [?do](https://forth-standard.org/standard/core/qDO)                     | TBD                                                   |
/// | [action-of](https://forth-standard.org/standard/core/ACTION-OF)         | TBD                                                   |
/// | [again](https://forth-standard.org/standard/core/AGAIN)                 | exactly ANS                                           |
/// | [buffer:](https://forth-standard.org/standard/core/BUFFERColon)         | TBD                                                   |
/// | [c&quot;](https://forth-standard.org/standard/core/Cq)                  | unimplemented                                         |
/// | [case](https://forth-standard.org/standard/core/CASE)                   | exactly ANS                                           |
/// | [compile,](https://forth-standard.org/standard/core/COMPILEComma)       | exactly ANS                                           |
/// | [defer](https://forth-standard.org/standard/core/DEFER)                 | unimplemented                                         |
/// | [defer!](https://forth-standard.org/standard/core/DEFERStore)           | unimplemented                                         |
/// | [defer@@](https://forth-standard.org/standard/core/DEFERFetch)          | unimplemented                                         |
/// | [endcase](https://forth-standard.org/standard/core/ENDCASE)             | exactly ANS                                           |
/// | [endof](https://forth-standard.org/standard/core/ENDOF)                 | exactly ANS                                           |
/// | [erase](https://forth-standard.org/standard/core/ERASE)                 | TBD                                                   |
/// | [false](https://forth-standard.org/standard/core/FALSE)                 | exactly ANS                                           |
/// | [hex](https://forth-standard.org/standard/core/HEX)                     | exactly ANS                                           |
/// | [holds](https://forth-standard.org/standard/core/HOLDS)                 | unimplemented                                         |
/// | [is](https://forth-standard.org/standard/core/IS)                       | unimplemented                                         |
/// | [marker](https://forth-standard.org/standard/core/MARKER)               | unimplemented                                         |
/// | [nip](https://forth-standard.org/standard/core/NIP)                     | exactly ANS                                           |
/// | [of](https://forth-standard.org/standard/core/OF)                       | exactly ANS                                           |
/// | [pad](https://forth-standard.org/standard/core/PAD)                     | unimplemented                                         |
/// | [parse](https://forth-standard.org/standard/core/PARSE)                 | obsolete                                              |
/// | [parse-name](https://forth-standard.org/standard/core/PARSE-NAME)       | exactly ANS                                           |
/// | [pick](https://forth-standard.org/standard/core/PICK)                   | exactly ANS                                           |
/// | [refill](https://forth-standard.org/standard/core/REFILL)               | unimplemented                                         |
/// | [restore-input](https://forth-standard.org/standard/core/RESTORE-INPUT) | unimplemented                                         |
/// | [roll](https://forth-standard.org/standard/core/ROLL)                   | exactly ANS                                           |
/// | [s\&quot;](https://forth-standard.org/standard/core/Seq)                | unimplemented                                         |
/// | [save-input](https://forth-standard.org/standard/core/SAVE-INPUT)       | unimplemented                                         |
/// | [source-id](https://forth-standard.org/standard/core/SOURCE-ID)         | unimplemented                                         |
/// | [to](https://forth-standard.org/standard/core/TO)                       | unimplemented                                         |
/// | [true](https://forth-standard.org/standard/core/TRUE)                   | exactly ANS                                           |
/// | [tuck](https://forth-standard.org/standard/core/TUCK)                   | exactly ANS                                           |
/// | [u.r](https://forth-standard.org/standard/core/UDotR)                   | unimplemented                                         |
/// | [u&gt;](https://forth-standard.org/standard/core/Umore)                 | exactly ANS                                           |
/// | [unused](https://forth-standard.org/standard/core/UNUSED)               | exactly ANS                                           |
/// | [value](https://forth-standard.org/standard/core/VALUE)                 | TBD                                                   |
/// | [within](https://forth-standard.org/standard/core/WITHIN)               | TBD                                                   |
/// | [\[compile\]](https://forth-standard.org/standard/core/BracketCOMPILE)  | obsolete                                              |
/// | <a href="https://forth-standard.org/standard/core/bs"> \ </a>           | unimplemented                                         |
///
/// | Shi word set | Description                                                                                                      |
/// | ------------ | ---------------------------------------------------------------------------------------------------------------- |
/// | h!           | Store halfword (16bit value).                                                                                    |
/// | h@           | Fetch halfword (16bit value).                                                                                    |
/// | inline       | Make the most recent definition an inline word.                                                                  |
/// | >text?       | Return true if compiler is currently compiling to text. Return false if compiler is currently compiling to data. |
/// | >data?       | Return true if compiler is currently compiling to data. Return false if compiler is currently compiling to text. |
/// | >text        | Compile to text.                                                                                                 |
/// | >data        | Compile to data.                                                                                                 |
// clang-format on
///
///
/// \page page_ambiguous_condition Ambiguous conditions
/// The following passage is taken from the
/// [standard](https://forth-standard.org/standard/usage) on ambiguous
/// conditions.
/// > When an ambiguous condition exists, a system may take one or more of the
/// > following actions:
/// > - ignore and continue;
/// > - display a message;
/// > - execute a particular word;
/// > - set interpretation state and begin text interpretation;
/// > - take other implementation-defined actions;
/// > - take implementation-dependent actions.
/// >
/// > The response to a particular ambiguous condition need not be the same
/// > under all circumstances.
///
/// Shi takes approach #1+2 and generally tries to ignore ambiguous
/// conditions as much as possible. There is the \ref SHI_ENABLE_PRINT macro
/// which gives you an error message in case you implemented \ref shi_printf but
/// other than that all definitions try to stick to their stack effect. This
/// produces garbage in case a word can can't fulfill its semantics but at least
/// it doesn't corrupt the return-stack.
