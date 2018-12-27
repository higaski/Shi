@ Data
@
@ \file   data.asm
@ \author Vincent Hamp
@ \date   02/12/2018

@ C/C++ interface
.global _s_shi_dstack, _e_shi_dstack, _s_shi_context

.section .data

@ ------------------------------------------------------------------------------
@ Stack
@ ------------------------------------------------------------------------------
.p2align 2                              @ Make sure the stack is 4-byte aligned (ldm and stm operations need 4-byte alignment)
_s_shi_dstack:                          @ Data-stack start
.space DSTACK_SIZE                      @ Data-stack size in bytes
_e_shi_dstack:                          @ Data-stack end

.p2align 2
_s_shi_context:                         @ Used to store the forth context
.space 12                               @ Save tos, dsp and lfp
_e_forth_context:

@ ------------------------------------------------------------------------------
@ Memory-space pointers
@ ------------------------------------------------------------------------------
.p2align 2
to_text_begin:
.word 0
data_begin:                             @ Pointer to data
.word 0
data_end:                               @ Used for reserving data for variables
.word 0
text_begin:                             @ Pointer to text
.word 0
text_end:
.word 0

@ Control-stack pointer
csp:                                    @ Inside loop: points to leave addresses from the current loop on the stack
.word _s_shi_dstack                     @ Inside case: points to endof addresses from the current case on the stack

@ ------------------------------------------------------------------------------
@ Contains address of link of the last definition
@ link initially has to point to the first definition in text, so that the very
@ first definition that gets created in data has an actual link back to text.
@ ------------------------------------------------------------------------------
link:                                   @ Last link
.word _s_shi_dict

@ ------------------------------------------------------------------------------
@ Core variables
@ ------------------------------------------------------------------------------
status:                                 @ Current state (state is taken as word)
.word 0                                 @ false: interpret, true: compile

.p2align 2                              @ Make sure src is 4-byte aligned
src:                                    @ Source
.word 0                                 @ c-addr
.word 0                                 @ u

in:                                     @ Index in terminal input buffer
.word 0

radix:                                  @ Determine current numerical base (base is taken as word)
.word 10

text_align:                             @ Text alignment
.byte 4

.p2align 1                              @ Make sure everything after is 2-byte aligned
