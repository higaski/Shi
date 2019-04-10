/// \page page_todo TODO
/// -# Die "defer" Familie an Wörter möcht ich glaub ich nicht unterstützen. Das
///    betrifft
///    - defer
///    - defer!
///    - defer@
///    - is (setzt bestehenden Namen auf x-beliebiges xt)
///    Das macht in meinen Augen nur Sinn wenn ma Forth als System nutzt und
///    irgnedwas dynamisch macht.
///
/// -# Eigene ERROR ein/ausschalt-Flags für:
///    - Stringlänge
///    - Stackoverflow generell
///    - Stackoverflow csp<->dsp
///    - Branchoffsets
///    - Undefined word
///    - interpreting compile-only word
///    - compiling interpret-only word
///
/// -# A paar wörter wie abort oder quit können im Fehlerfall den return-stack
///    löschen. Das klingt prinzipiell nach am coolen feature, würd aber heißn
///    dass ma in stackpointer bei am context-switch speichern muss.
///
/// -# I unterscheid zw. c-addr u und token-addr und token-u ... sonst kennt si
///    ka sau aus
///
///
/// \page page_interpret Interpret loop
/// This graph is a more detailed version of the one on the \ref index mainpage.
///
/// \dot
/// digraph G {
///
///   ratio=auto; node[fontsize=12]; newrank=true;
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

//*/mod
/// mod
// abort
// fill
// fm/mod
// immediate
// m*
// move
// quit
// s>d
// sign
// sm/rem
// um*
// um/mod
//
//:noname weils wohl auch ohne defer nutzbar is?
//?do
// action_of...? wtf macht des
// buffer: super wichtig!!!
// erase
// is ... ?
// marker (klingt mega hart)
// value
// within
