/// Shi
///
/// \file   shi.hpp
/// \author	Vincent Hamp
/// \date   27/07/2016

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
/// extensive C/C++ API. The functions shi::evaluate and shi_evaluate can be
/// used to enter the classic interpretation loop. This reflects what the word
/// [evaluate](https://forth-standard.org/standard/core/EVALUATE) normally does.
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
/// | \subpage page_init                | Initialization                                    |
/// | \subpage page_control_structures  | Control structures                                |
/// | \subpage page_interpret           | Interpretation loop in detail                     |
/// | \subpage page_wordlist            | Wordlist                                          |
/// | \subpage page_ambiguous_condition | Ambiguous conditions                              |
/// | \subpage page_todo                | d                                                 |
/// | \subpage page_lerp                | c                                                 |
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
///
/// \page page_variables Variables
/// There are a couple of states Shi needs to track internally. When evaluating
/// the total ram usage these variables have to be taken into consideration and
/// added to the user-defined stack size. The table below contains them all
/// including a short description and their size in bytes.
///
// clang-format off
/// \page page_variables Variables
/// | Symbol          | Description                                                                        | Size [b]     |
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
/// | link            | Contains address of link of the last definition in ram                             | 4            |
/// | status          | Current state (state is taken as word) <br><!--                                    |              |
/// |                 | --> false: interpret, true: compile                                                | 4            |
/// | src             | Source (address and length)                                                        | 8            |
/// | in              | Index in terminal input buffer                                                     | 4            |
/// | radix           | Determine current numerical base (base is taken as word)                           | 4            |
/// | text_align      | User defined alignment for flash                                                   | 1            |
// clang-format on
///
/// Most variables should be more or less evident with some exceptions:
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
/// \page page_init Initialization
/// To initialize Shi the functions shi::init or shi_init can be used. Both
/// functions take a struct which contains the begin and end addresses of the
/// data-spaces as well as the necessary text alignment for compilation to
/// flash. Passing addresses and alignment for text is completely optional and
/// can simply be set to 0 if not needed.
///
/// Besides applying the passed addresses there are three things happening
/// during initialization.
///
/// **1.Sweep text**<br>
/// The whole dictionary is searched for definitions which need to reserve ram.
/// The necessary amount of ram memory is taken from the end of data-space.
/// Afterwards the last found link is saved as beginning of the text data-space.
/// At this point the last link might either be the last core dictionary entry
/// or the last user dictionary entry depending on whether the user has already
/// extended the dictionary or not.
///
/// **2.Fill data**<br>
/// Shi does not rely on whether the data-space passed in is zero-initialized or
/// not. In any case it gets overwritten by the value defined by \ref
/// SHI_ERASED_WORD. By default that's 0xFFFFFFFF which mimics what most cleared
/// flash devices are.
///
/// **3.Set context**<br>
/// Initializing the Shi context which means initializing the following
/// registers
///
/// ```asm
/// tos .req r6                             @ Top of stack
/// dsp .req r7                             @ Data-stack pointer
/// lfp .req r8                             @ Literal-folding pointer
/// ```
///
/// *tos* gets initialized with '*', *dsp* with the stack end and *lfp* with 0.
///
///
/// \page page_control_structures Control structures
/// Blabla
///
/// if...else...then
///
/// case...of...endof...endcase
///
/// begin...until
/// begin...while...repeat
///
/// do...loop/+loop
/// do...if...leave then...loop/+loop
///
/// if -> ( -- orig1 )
/// else -> ( orig1 -- orig2 )
/// then -> ( orig1 | orig2 -- )
///
/// begin -> ( -- dest )
/// until -> ( dest -- )
///
/// begin -> ( -- dest )
/// while -> ( dest -- orig dest )
/// repeat -> ( orig dest -- )
///
/// Dokumentieren dass else bei orig des unterste bit setzt...
///
/// Ausserdem dass es nur mehr 1x conditional branch funktion gibt und das
/// "template" fürn OPCODE vorher am stack landet
///
///
// clang-format off
/// \page page_wordlist Wordlist
/// | Core word set | Description |
/// | ------------- | ------------- |
/// | [!](https://forth-standard.org/standard/core/Store)                     | exactly ANS |
/// | [#](https://forth-standard.org/standard/core/num)                       | unimplemented |
/// | [#&gt;](https://forth-standard.org/standard/core/num-end)               | unimplemented |
/// | [#s](https://forth-standard.org/standard/core/numS)                     | unimplemented |
/// | ['](https://forth-standard.org/standard/core/Tick)                      | exactly ANS |
/// | [(](https://forth-standard.org/standard/core/p)                         | unimplemented |
/// | [*](https://forth-standard.org/standard/core/Times)                     | exactly ANS |
/// | [*&frasl;](https://forth-standard.org/standard/core/TimesDiv)           | exactly ANS |
/// | [*&frasl;mod](https://forth-standard.org/standard/core/TimesDivMOD)     | unimplemented |
/// | [+](https://forth-standard.org/standard/core/Plus)                      | exactly ANS |
/// | [+!](https://forth-standard.org/standard/core/PlusStore)                | exactly ANS |
/// | [+loop](https://forth-standard.org/standard/core/PlusLOOP)              | exactly ANS |
/// | [,](https://forth-standard.org/standard/core/Comma)                     | exactly ANS |
/// | [-](https://forth-standard.org/standard/core/Minus)                     | exactly ANS |
/// | [.](https://forth-standard.org/standard/core/d)                         | unimplemented |
/// | [.&quot;](https://forth-standard.org/standard/core/Dotq)                | unimplemented |
/// | [&frasl;](https://forth-standard.org/standard/core/Div)                 | exactly ANS |
/// | [&frasl;mod](https://forth-standard.org/standard/core/DivMOD)           | unimplemented |
/// | [0&lt;](https://forth-standard.org/standard/core/Zeroless)              | exactly ANS |
/// | [0=](https://forth-standard.org/standard/core/ZeroEqual)                | exactly ANS |
/// | [1+](https://forth-standard.org/standard/core/OnePlus)                  | exactly ANS |
/// | [1-](https://forth-standard.org/standard/core/OneMinus)                 | exactly ANS |
/// | [2!](https://forth-standard.org/standard/core/TwoStore)                 | exactly ANS |
/// | [2*](https://forth-standard.org/standard/core/TwoTimes)                 | exactly ANS |
/// | [2&frasl;](https://forth-standard.org/standard/core/TwoDiv)             | exactly ANS |
/// | [2@@](https://forth-standard.org/standard/core/TwoFetch)                | exactly ANS |
/// | [2drop](https://forth-standard.org/standard/core/TwoDROP)               | exactly ANS |
/// | [2dup](https://forth-standard.org/standard/core/TwoDUP)                 | exactly ANS |
/// | [2over](https://forth-standard.org/standard/core/TwoOVER)               | exactly ANS |
/// | [2swap](https://forth-standard.org/standard/core/TwoSWAP)               | exactly ANS |
/// | [:](https://forth-standard.org/standard/core/Colon)                     | exactly ANS |
/// | [;](https://forth-standard.org/standard/core/Semi)                      | exactly ANS |
/// | [&lt;](https://forth-standard.org/standard/core/less)                   | exactly ANS |
/// | [&lt;#](https://forth-standard.org/standard/core/num-start)             | unimplemented |
/// | [=](https://forth-standard.org/standard/core/Equal)                     | exactly ANS |
/// | [&gt;](https://forth-standard.org/standard/core/more)                   | exactly ANS |
/// | [&gt;body](https://forth-standard.org/standard/core/toBODY)             | exactly ANS |
/// | [&gt;in](https://forth-standard.org/standard/core/toIN)                 | exactly ANS |
/// | [&gt;number](https://forth-standard.org/standard/core/toNUMBER)         | unimplemented |
/// | [&gt;r](https://forth-standard.org/standard/core/toR)                   | exactly ANS |
/// | [?dup](https://forth-standard.org/standard/core/qDUP)                   | exactly ANS |
/// | [@@](https://forth-standard.org/standard/core/Fetch)                    | exactly ANS |
/// | [abort](https://forth-standard.org/standard/core/ABORT)                 | would require tracking of return-stack |
/// | [abort&quot;](https://forth-standard.org/standard/core/ABORTq)          | unimplemented |
/// | [abs](https://forth-standard.org/standard/core/ABS)                     | exactly ANS |
/// | [accept](https://forth-standard.org/standard/core/ACCEPT)               | unimplemented |
/// | [align](https://forth-standard.org/standard/core/ALIGN)                 | exactly ANS |
/// | [aligned](https://forth-standard.org/standard/core/ALIGNED)             | exactly ANS |
/// | [allot](https://forth-standard.org/standard/core/ALLOT)                 | exactly ANS |
/// | [and](https://forth-standard.org/standard/core/AND)                     | exactly ANS |
/// | [base](https://forth-standard.org/standard/core/BASE)                   | exactly ANS |
/// | [begin](https://forth-standard.org/standard/core/BEGIN)                 | exactly ANS |
/// | [bl](https://forth-standard.org/standard/core/BL)                       | unimplemented |
/// | [c!](https://forth-standard.org/standard/core/CStore)                   | exactly ANS |
/// | [c,](https://forth-standard.org/standard/core/CComma)                   | exactly ANS |
/// | [c@@](https://forth-standard.org/standard/core/CFetch)                  | exactly ANS |
/// | [cell+](https://forth-standard.org/standard/core/CELLPlus)              | exactly ANS |
/// | [cells](https://forth-standard.org/standard/core/CELLS)                 | exactly ANS |
/// | [char](https://forth-standard.org/standard/core/CHAR)                   | unimplemented |
/// | [char+](https://forth-standard.org/standard/core/CHARPlus)              | exactly ANS |
/// | [chars](https://forth-standard.org/standard/core/CHARS)                 | exactly ANS |
/// | [constant](https://forth-standard.org/standard/core/CONSTANT)           | exactly ANS |
/// | [count](https://forth-standard.org/standard/core/COUNT)                 | unimplemented |
/// | [cr](https://forth-standard.org/standard/core/CR)                       | unimplemented |
/// | [create](https://forth-standard.org/standard/core/CREATE)               | exactly ANS |
/// | [decimal](https://forth-standard.org/standard/core/DECIMAL)             | exactly ANS |
/// | [depth](https://forth-standard.org/standard/core/DEPTH)                 | exactly ANS |
/// | [do](https://forth-standard.org/standard/core/DO)                       | exactly ANS |
/// | [does&gt;](https://forth-standard.org/standard/core/DOES)               | exactly ANS |
/// | [drop](https://forth-standard.org/standard/core/DROP)                   | exactly ANS |
/// | [dup](https://forth-standard.org/standard/core/DUP)                     | exactly ANS |
/// | [else](https://forth-standard.org/standard/core/ELSE)                   | exactly ANS |
/// | [emit](https://forth-standard.org/standard/core/EMIT)                   | unimplemented |
/// | [environment?](https://forth-standard.org/standard/core/ENVIRONMENTq)   | unimplemented |
/// | [evaluate](https://forth-standard.org/standard/core/EVALUATE)           | unimplemented |
/// | [execute](https://forth-standard.org/standard/core/EXECUTE)             | exactly ANS |
/// | [exit](https://forth-standard.org/standard/core/EXIT)                   | exactly ANS |
/// | [fill](https://forth-standard.org/standard/core/FILL)                   | unimplemented |
/// | [find](https://forth-standard.org/standard/core/FIND)                   | not ANS (returns xt flags instead of xt 1 or xt -1) |
/// | [fm&frasl;mod](https://forth-standard.org/standard/core/FMDivMOD)       | unimplemented |
/// | [here](https://forth-standard.org/standard/core/HERE)                   | exactly ANS |
/// | [hold](https://forth-standard.org/standard/core/HOLD)                   | unimplemented |
/// | [i](https://forth-standard.org/standard/core/I)                         | exactly ANS |
/// | [if](https://forth-standard.org/standard/core/IF)                       | exactly ANS |
/// | [immediate](https://forth-standard.org/standard/core/IMMEDIATE)         | unimplemented |
/// | [invert](https://forth-standard.org/standard/core/INVERT)               | exactly ANS |
/// | [j](https://forth-standard.org/standard/core/J)                         | exactly ANS |
/// | [key](https://forth-standard.org/standard/core/KEY)                     | unimplemented |
/// | [leave](https://forth-standard.org/standard/core/LEAVE)                 | exactly ANS |
/// | [literal](https://forth-standard.org/standard/core/LITERAL)             | exactly ANS |
/// | [loop](https://forth-standard.org/standard/core/LOOP)                   | exactly ANS |
/// | [lshift](https://forth-standard.org/standard/core/LSHIFT)               | exactly ANS |
/// | [m*](https://forth-standard.org/standard/core/MTimes)                   | unimplemented |
/// | [max](https://forth-standard.org/standard/core/MAX)                     | exactly ANS |
/// | [min](https://forth-standard.org/standard/core/MIN)                     | exactly ANS |
/// | [mod](https://forth-standard.org/standard/core/MOD)                     | exactly ANS |
/// | [move](https://forth-standard.org/standard/core/MOVE)                   | unimplemented |
/// | [negate](https://forth-standard.org/standard/core/NEGATE)               | exactly ANS |
/// | [or](https://forth-standard.org/standard/core/OR)                       | exactly ANS |
/// | [over](https://forth-standard.org/standard/core/OVER)                   | exactly ANS |
/// | [postpone](https://forth-standard.org/standard/core/POSTPONE)           | exactly ANS |
/// | [quit](https://forth-standard.org/standard/core/QUIT)                   | would require tracking of return-stack |
/// | [r&gt;](https://forth-standard.org/standard/core/Rfrom)                 | exactly ANS |
/// | [r@@](https://forth-standard.org/standard/core/RFetch)                  | exactly ANS |
/// | [recurse](https://forth-standard.org/standard/core/RECURSE)             | exactly ANS |
/// | [repeat](https://forth-standard.org/standard/core/REPEAT)               | exactly ANS |
/// | [rot](https://forth-standard.org/standard/core/ROT)                     | exactly ANS |
/// | [rshift](https://forth-standard.org/standard/core/RSHIFT)               | exactly ANS |
/// | [s&quot;](https://forth-standard.org/standard/core/Sq)                  | unimplemented |
/// | [s&gt;d](https://forth-standard.org/standard/core/StoD)                 | unimplemented |
/// | [sign](https://forth-standard.org/standard/core/SIGN)                   | unimplemented |
/// | [sm&frasl;rem](https://forth-standard.org/standard/core/SMDivREM)       | unimplemented |
/// | [source](https://forth-standard.org/standard/core/SOURCE)               | exactly ANS |
/// | [space](https://forth-standard.org/standard/core/SPACE)                 | unimplemented |
/// | [spaces](https://forth-standard.org/standard/core/SPACES)               | unimplemented |
/// | [state](https://forth-standard.org/standard/core/STATE)                 | exactly ANS |
/// | [swap](https://forth-standard.org/standard/core/SWAP)                   | exactly ANS |
/// | [then](https://forth-standard.org/standard/core/THEN)                   | exactly ANS |
/// | [type](https://forth-standard.org/standard/core/TYPE)                   | unimplemented |
/// | [u.](https://forth-standard.org/standard/core/Ud)                       | unimplemented |
/// | [u&lt;](https://forth-standard.org/standard/core/Uless)                 | exactly ANS |
/// | [um*](https://forth-standard.org/standard/core/UMTimes)                 | unimplemented |
/// | [um&frasl;mod](https://forth-standard.org/standard/core/UMDivMOD)       | unimplemented |
/// | [unloop](https://forth-standard.org/standard/core/UNLOOP)               | exactly ANS |
/// | [until](https://forth-standard.org/standard/core/UNTIL)                 | exactly ANS |
/// | [variable](https://forth-standard.org/standard/core/VARIABLE)           | exactly ANS |
/// | [while](https://forth-standard.org/standard/core/WHILE)                 | exactly ANS |
/// | [word](https://forth-standard.org/standard/core/WORD)                   | obsolete |
/// | [xor](https://forth-standard.org/standard/core/XOR)                     | exactly ANS |
/// | [\[](https://forth-standard.org/standard/core/Bracket)                  | exactly ANS |
/// | [\['\]](https://forth-standard.org/standard/core/BracketTick)           | exactly ANS |
/// | [\[char\]](https://forth-standard.org/standard/core/BracketCHAR)        | unimplemented |
/// | [\]](https://forth-standard.org/standard/core/right-bracket)            | exactly ANS |
/// | [.(](https://forth-standard.org/standard/core/Dotp)                     | unimplemented |
/// | [.r](https://forth-standard.org/standard/core/DotR)                     | unimplemented |
/// | [0&lt;&gt;](https://forth-standard.org/standard/core/Zerone)            | exactly ANS |
/// | [0&gt;](https://forth-standard.org/standard/core/Zeromore)              | exactly ANS |
/// | [2&gt;r](https://forth-standard.org/standard/core/TwotoR)               | exactly ANS |
/// | [2r&gt;](https://forth-standard.org/standard/core/TwoRfrom)             | exactly ANS |
/// | [2r@@](https://forth-standard.org/standard/core/TwoRFetch)              | exactly ANS |
/// | [:noname](https://forth-standard.org/standard/core/ColonNONAME)         | unimplemented |
/// | [&lt;&gt;](https://forth-standard.org/standard/core/ne)                 | exactly ANS |
/// | [?do](https://forth-standard.org/standard/core/qDO)                     | unimplemented |
/// | [action-of](https://forth-standard.org/standard/core/ACTION-OF)         | unimplemented |
/// | [again](https://forth-standard.org/standard/core/AGAIN)                 | exactly ANS |
/// | [buffer:](https://forth-standard.org/standard/core/BUFFERColon)         | unimplemented |
/// | [c&quot;](https://forth-standard.org/standard/core/Cq)                  | unimplemented |
/// | [case](https://forth-standard.org/standard/core/CASE)                   | exactly ANS |
/// | [compile,](https://forth-standard.org/standard/core/COMPILEComma)       | exactly ANS |
/// | [defer](https://forth-standard.org/standard/core/DEFER)                 | unimplemented |
/// | [defer!](https://forth-standard.org/standard/core/DEFERStore)           | unimplemented |
/// | [defer@@](https://forth-standard.org/standard/core/DEFERFetch)          | unimplemented |
/// | [endcase](https://forth-standard.org/standard/core/ENDCASE)             | exactly ANS |
/// | [endof](https://forth-standard.org/standard/core/ENDOF)                 | exactly ANS |
/// | [erase](https://forth-standard.org/standard/core/ERASE)                 | unimplemented |
/// | [false](https://forth-standard.org/standard/core/FALSE)                 | exactly ANS |
/// | [hex](https://forth-standard.org/standard/core/HEX)                     | exactly ANS |
/// | [holds](https://forth-standard.org/standard/core/HOLDS)                 | unimplemented |
/// | [is](https://forth-standard.org/standard/core/IS)                       | unimplemented |
/// | [marker](https://forth-standard.org/standard/core/MARKER)               | unimplemented |
/// | [nip](https://forth-standard.org/standard/core/NIP)                     | exactly ANS |
/// | [of](https://forth-standard.org/standard/core/OF)                       | exactly ANS |
/// | [pad](https://forth-standard.org/standard/core/PAD)                     | unimplemented |
/// | [parse](https://forth-standard.org/standard/core/PARSE)                 | obsolete |
/// | [parse-name](https://forth-standard.org/standard/core/PARSE-NAME)       | exactly ANS |
/// | [pick](https://forth-standard.org/standard/core/PICK)                   | exactly ANS |
/// | [refill](https://forth-standard.org/standard/core/REFILL)               | unimplemented |
/// | [restore-input](https://forth-standard.org/standard/core/RESTORE-INPUT) | unimplemented |
/// | [roll](https://forth-standard.org/standard/core/ROLL)                   | exactly ANS |
/// | [s\&quot;](https://forth-standard.org/standard/core/Seq)                | unimplemented |
/// | [save-input](https://forth-standard.org/standard/core/SAVE-INPUT)       | unimplemented |
/// | [source-id](https://forth-standard.org/standard/core/SOURCE-ID)         | unimplemented |
/// | [to](https://forth-standard.org/standard/core/TO)                       | unimplemented |
/// | [true](https://forth-standard.org/standard/core/TRUE)                   | exactly ANS |
/// | [tuck](https://forth-standard.org/standard/core/TUCK)                   | exactly ANS |
/// | [u.r](https://forth-standard.org/standard/core/UDotR)                   | unimplemented |
/// | [u&gt;](https://forth-standard.org/standard/core/Umore)                 | exactly ANS |
/// | [unused](https://forth-standard.org/standard/core/UNUSED)               | exactly ANS |
/// | [value](https://forth-standard.org/standard/core/VALUE)                 | unimplemented |
/// | [within](https://forth-standard.org/standard/core/WITHIN)               | unimplemented |
/// | [\[compile\]](https://forth-standard.org/standard/core/BracketCOMPILE)  | obsolete |
/// | <a href="https://forth-standard.org/standard/core/bs"> \ </a>           | unimplemented |
///
/// | Shi word set | unimplemented |
/// | ------------ | ------------- |
/// | c-variable   | unimplemented |
/// | >text?       | unimplemented |
/// | >data?       | unimplemented |
/// | >text        | unimplemented |
/// | >data        | unimplemented |
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
///
/// 'y1 y2 y1 - x x1 - * x2 x1 - / +'
///
/// 'x x1 x2 y1 y2'
///
/// over
///
/// 'x x1 x2 y1 y2 y1'
///
/// -
///
/// 'x x1 x2 y1 (y2-y1)'
///
/// 4 roll
///
/// 'x1 x2 y1 (y2-y1) x'
///
/// 4 pick
///
/// 'x1 x2 y1 (y2-y1) x x1'
///
/// -
///
/// 'x1 x2 y1 (y2-y1) (x-x1)'
///
/// *
///
/// 'x1 x2 y1 (y2-y1)*(x-x1)'
///
/// 2swap
///
/// 'y1 (y2-y1)*(x-x1) x1 x2'
///
/// swap
///
/// 'y1 (y2-y1)*(x-x1) x2 x1'
///
/// -
///
/// 'y1 (y2-y1)*(x-x1) (x2-x1)'
///
/// /
///
/// 'y1 (y2-y1)*(x-x1)/(x2-x1)'
///
/// +
///
/// ... done
/// : lerp over - 4 roll 4 pick - * 2swap swap - / + ;

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

///
#define SHI_ENABLE_STACKOVERFLOW_ERROR          1

///
#define SHI_ENABLE_UNDEFINED_WORD_ERROR         1

///
#define SHI_ENABLE_REDEFINED_WORD_ERROR         1

///
#define SHI_ENABLE_INTERPRET_COMPILE_ONLY_ERROR 1

///
#define SHI_ENABLE_COMPILE_INTERPRET_ONLY_ERROR 1

///
#define SHI_ENABLE_BRANCH_OFFSET_ERROR          1

// Enable words individually
// Core words
#define SHI_ENABLE_STORE           1
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

// Shi words
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
extern uint32_t shi_context;

void shi_evaluate_asm(char const* str, size_t len);
void shi_c_variable_asm(char const* name, size_t len);
void shi_clear_asm();
void_fp shi_tick_asm(char const* str, size_t len);

#  ifdef __cplusplus
}
#  endif

// C only
#  ifndef __cplusplus

static inline void shi_push_s(int32_t t) {
  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "str tos, [dsp, #-4]! \n"
               "movs tos, %1 \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&shi_context), "r"(t)
               : "cc", "memory", "r0", "r1");
}

static inline void shi_push_d(int64_t t) {
  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "ldrd r2, r3, [%1] \n"
               "strd r2, tos, [dsp, #-8]! \n"
               "movs tos, r3 \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&shi_context), "r"(&t)
               : "cc", "memory", "r0", "r1", "r2", "r3");
}

static inline int32_t shi_pop_s() {
  int32_t t;

  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%1] \n"
               "movs %0, tos \n"
               "ldmia dsp!, {tos} \n"
               "strd tos, dsp, [%1] \n"
               : "=&r"(t)
               : "r"(&shi_context)
               : "cc", "memory", "r0", "r1");

  return t;
}

static inline int32_t shi_pop_d() {
  int64_t t;

  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "ldrd r2, r3, [dsp], #8 \n"
               "strd r2, tos, [%1] \n"
               "movs tos, r3 \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&shi_context), "r"(&t)
               : "cc", "memory", "r0", "r1", "r2", "r3");

  return t;
}

/// Remove element from the top of the stack
static inline void shi_pop() {
  asm volatile("tos .req r0 \n"
               "dsp .req r1 \n"
               "ldrd tos, dsp, [%0] \n"
               "ldmia dsp!, {tos} \n"
               "strd tos, dsp, [%0] \n"
               :
               : "r"(&shi_context)
               : "cc", "memory", "r0", "r1");
}

static inline int32_t shi_top(size_t offset) {
  int32_t t;

  asm volatile("cmp %2, #0 \n"
               "iteee eq \n"
               "ldreq %0, [%1] \n"
               "subne r1, %2, #1 \n"
               "ldrne r0, [%1, #4] \n"
               "ldrne %0, [r0, r1, lsl #2] \n"
               : "=r"(t)
               : "r"(&shi_context), "r"(offset)
               : "cc", "memory", "r0", "r1");

  return t;
}

/// Returns the number of elements on the stack
///
/// \return Number of elements on stack
static inline size_t shi_depth() {
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
static inline size_t shi_size() {
  return shi_depth();
}

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
static inline void shi_init(shi_init_t s) {
  shi_init_asm(&s);
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
static inline void shi_evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

/// Call of word evaluate
///
/// \param  str Pointer to the null-terminated byte string
/// \param  len Length of the null-terminated string
static inline void shi_evaluate_len(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

static inline void shi_c_variable(char const* str, void* adr) {
  push((int32_t)(adr));
  shi_c_variable_asm(str, strlen(str));
}

/// Clear stack
static inline void shi_clear() {
  shi_clear_asm();
}

static inline void_fp shi_tick(char const* str) {
  return shi_tick_asm(str, strlen(str));
}

static inline void_fp shi_tick_len(char const* str, size_t len) {
  return shi_tick_asm(str, len);
}

static inline void shi_call_tick(void_fp fp) {
  if (fp)
    asm volatile("ldmia %0, {r6, r7, r8} \n"
                 "push {%0} \n"
                 "blx %1 \n"
                 "pop {%0} \n"
                 "stmia %0, {r6, r7, r8} \n"
                 :
                 : "r"(&shi_context), "r"(fp)
                 : "cc", "memory", "r6", "r7", "r8", "lr");
}

// C++ only
#  else

#    include <tuple>
#    include <type_traits>

namespace shi {

// Shouldn't that be part of C++20?
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
  using std::addressof, std::is_arithmetic_v, std::is_pointer_v;
  using V = remove_cvref_t<T>;

  static_assert(sizeof(V) <= SHI_STACK_SIZE);

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
    ;  // basically memcpy here?
}

/// Add elements to the top of the stack
///
/// \tparam Ts  Types of elements to push
/// \param  ts  Values to push
template<typename... Ts>
inline void push(Ts&&... ts) {
  static_assert((0 + ... + sizeof(Ts)) <= SHI_STACK_SIZE);

  (push(std::forward<Ts>(ts)), ...);
}

/// Remove element from the top of the stack
///
/// \tparam T   Type of element to pop
/// \return Value
template<typename T>
inline remove_cvref_t<T> pop() {
  using std::addressof, std::is_arithmetic_v, std::is_pointer_v;
  using V = remove_cvref_t<T>;

  static_assert(sizeof(V) <= SHI_STACK_SIZE);

  T t;

  if constexpr (sizeof(V) <= 4 && (is_arithmetic_v<V> || is_pointer_v<V> ||
                                   is_reference_wrapper_v<V>))
    asm volatile("tos .req r0 \n"
                 "dsp .req r1 \n"
                 "ldrd tos, dsp, [%1] \n"
                 "movs %0, tos \n"
                 "ldmia dsp!, {tos} \n"
                 "strd tos, dsp, [%1] \n"
                 : "=&r"(t)
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
    ;  // basically memcpy here?

  return t;
}

/// Remove elements from the top of the stack
///
/// \tparam Ts  Types of element to pop
/// \return Values
template<typename... Ts, typename = std::enable_if_t<(sizeof...(Ts) > 1)>>
inline std::tuple<remove_cvref_t<Ts>...> pop() {
  using std::tuple;

  static_assert((0 + ... + sizeof(Ts)) <= SHI_STACK_SIZE);

  return tuple<Ts...>{pop<Ts>()...};
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
  int32_t t;

  asm volatile("cmp %2, #0 \n"
               "iteee eq \n"
               "ldreq %0, [%1] \n"
               "subne r1, %2, #1 \n"
               "ldrne r0, [%1, #4] \n"
               "ldrne %0, [r0, r1, lsl #2] \n"
               : "=r"(t)
               : "r"(&shi_context), "r"(offset)
               : "cc", "memory", "r0", "r1");

  return t;
}

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

template<typename T>
void c_variable(char const* str, T adr) {
  using std::is_pointer_v;

  static_assert(sizeof(T) <= 4 &&
                (is_pointer_v<T> || is_reference_wrapper_v<T>));

  push(adr);
  shi_c_variable_asm(str, strlen(str));
}

/// Clear stack
inline void clear() {
  shi_clear_asm();
}

struct word {
  constexpr word() = default;
  word(char const* str) : fp{shi_tick_asm(str, strlen(str))} {}
  word(char const* str, size_t len) : fp{shi_tick_asm(str, len)} {}

  template<typename... Ts>
  word& operator()(Ts&&... ts) {
    using std::forward;

    if (fp) {
      push(forward<Ts>(ts)...);

      asm volatile("ldmia %0, {r6, r7, r8} \n"
                   "push {%0} \n"
                   "blx %1 \n"
                   "pop {%0} \n"
                   "stmia %0, {r6, r7, r8} \n"
                   :
                   : "r"(&shi_context), "r"(fp)
                   : "cc", "memory", "r6", "r7", "r8", "lr");
    }

    return *this;
  }

  template<typename T>
  operator T() {
    return pop<T>();
  }

  template<typename... Ts>
  operator std::tuple<Ts...>() {
    return pop<Ts...>();
  }

private:
  void_fp fp{nullptr};
};

inline void operator"" _s(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

template<typename T, T... Cs>
word operator""_w() {
  static constexpr char c[]{Cs...};
  static auto literal_word{word(c, sizeof...(Cs))};
  return literal_word;
}

}  // namespace shi

#  endif

#endif
