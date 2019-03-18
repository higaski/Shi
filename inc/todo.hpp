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
/// \page page_doc_stuff Doc stuff
/// Data-stack -> Selbst gemanaged via tos und dsp, normal im RAM<br>
/// Return-stack -> ECHTER "C" stack managed via sp... (so DONT FUCK THAT UP)
///
/// Der ganze Scheiss is so kompliziert...
///
/// Es gibt im Prinzip 3x Speicherbereiche:
/// - Core Dictionary im FLASH (abgesehn vom letzten Eintrag)
/// - User Dictionary im RAM
/// - User Dictionary im FLASH
///
/// Des Core und User Dictionary wird von vorne nach hinten durchsucht. Der
/// letzte Eintrag im Core Dictionary liegt allerdings im RAM und zeigt nach dem
/// Initialisieren ins Leere (0xFFFFFFFF). Nur wenn der User einen FLASH Bereich
/// für die Nutzung übergibt, dann wird der letzte Core Dictionary Eintrag
/// geupdatet und zeigt an den Anfang des User Dictionary im FLASH.
///
/// Des User Dictionary im RAM wird von hinten nach vorne durchsucht. Das heißt
/// die Reihenfolge is:
/// - User RAM -> Core FLASH -> User FLASH
/// Die Suche nach Eintraegen beginnt IMMER mit "link". link wird auch nur
/// geupdatet wenn ein Eintrag ins RAM gschrieben wird. link is somit quasi
/// immer des Ende vom Faden wo ma zu Suchen beginnt.
///
///
/// sweep_text<br>
/// Bei da Init wird die Funktion sweep_text aufgerufen. Die rennt das ganze
/// Dictionary durch und prüft ob Einträge RAM reservieren müssen wie es zum
/// Beispiel das Wort "variable" tut. Jener RAM wird hinten vom übergebenen RAM
/// Bereich genommen! Und nacher wird data_end entsprechend angepasst.
/// Außerdem setzt des Ding den letzten Pointer am Dict-Ende richtig ;)
///
/// csp<br>
/// csp is im Prinzip ein Stackpointer... Im Gegensatz zum normalen Stackpointer
/// (dsp) wächst der aber nicht nach unten in den Stack sondern fängt unten an
/// und wächst nach oben. Von der Richtung her wird "Struktur" Info am Stack
/// abgelegt, wie mas für "leave" (break aus loop) oder "endof" (break aus
/// switch-case) braucht. Die stellen dabei die Ausnahme dar, weils von
/// "leave(s)" und "endof(s)" beliebig viele geben kann. Alle anderen
/// Kontrollstrukturen laufen übern normalen Stack.
///
/// Bezüglich Nummern...<br>
/// HEX wird AUSSCHLIESSLICH!!!! in Großbuchstaben unterstützt!!!
///
/// Fold Flags gibts nur für Words, die pure sind, sprich keine Nebeneffekte
/// haben. Die Anzahl der Eingangsparameter is fürs Falten wichtig. Sprich 1x
/// Eingangsparameter -> FOLDS_1, 2x Eingangsparameter -> FOLDS_2 usw.
///
/// Es gibt 4x Sprungfunktionen:
/// - b_comma
/// - beq_comma
/// - blt_comma
/// - bne_comma
/// Die Sprünge von orig nach dest compilieren. Innerhalb der Funktion wird
/// manchmal data_begin kurzzeitig überschrieben, damit die anderen Comma
/// Funktionen den Sprung an die richtige Stelle schreiben.
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
///
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
/// \page page_eval Evaluate
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
