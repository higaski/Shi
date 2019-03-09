/// \page page_todo TODO
/// -# Find die 4-byte vergleichsvariante streichen
///
/// -# Die "defer" Familie an Wörter möcht ich glaub ich nicht unterstützen. Das
///    betrifft
///    - defer
///    - defer!
///    - defer@
///    - is (setzt bestehenden Namen auf x-beliebiges xt)
///    Das macht in meinen Augen nur Sinn wenn ma Forth als System nutzt und
///    irgnedwas dynamisch macht.
///
/// \page page_doc_stuff Doc stuff
/// Data-stack -> Selbst gemanaged via tos und dsp, normal im RAM<br>
/// Return-stack -> ECHTER "C" stack managed via sp... (so DONT FUCK THAT UP)
///
/// Die Funktion "shi_set_memory_space_pointer_flash" wird aber weiterhin
/// gebraucht. Weil wenn da User neue Funktionen im FLASH anlegt, dann muessen
/// ma es User Dictionary durchsuchen und schaun ob dort bereits Einträge
/// vorhanden sind.
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
/// reverse_ram<br>
/// Bei da Init wird die Funktion reserve_ram aufgerufen. Die rennt das ganze
/// Dictionary durch und prüft ob Einträge RAM reservieren müssen wie es zum
/// Beispiel das Wort "variable" tut. Jener RAM wird hinten vom übergebenen RAM
/// Bereich genommen! Und nacher wird data_end entsprechend angepasst.
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
// clang-format off
/// \page page_doc_stuff Doc stuff
/// | Symbols         | Responsibility                                                                     |
/// | ----------------| ---------------------------------------------------------------------------------- |
/// | _s_shi_dstack   | Symbol at start of stack                                                           |
/// | _e_shi_dstack   | Symbol at end of stack                                                             |
/// | _s_shi_context  | Symbol at start of context (tos, dsp and lfp)                                      |
/// | _s_shi_context  | Symbol at end of context                                                           |
/// | data_begin      | Pointer to ram begin                                                               |
/// | data_end        | Pointer to ram end, used for reserving ram for variables                           |
/// | text_begin      | Pointer to flash begin                                                             |
/// | text_end        | Pointer to flash end                                                               |
/// | csp             | Inside loop: points to leave addresses from the current loop on the stack <br><!-- |
/// |                 | --> Inside case: points to end of addresses from the current case on the stack     |
/// | link            | Contains address of link of the last definition                                    |
/// | status          | Current state (state is taken as word) <br><!--                                    |
/// |                 | --> false: interpret, true: compile                                                |
/// | status_compiler | Current compiler state <br><!--                                                    |
/// |                 | --> false: compile to ram, true: compile to flash                                  |
/// | src             | Source (address and length)                                                        |
/// | in              | Index in terminal input buffer                                                     |
/// | radix           | Determine current numerical base (base is taken as word)                           |
// clang-format on
/// \page page_doc_stuff Doc stuff
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
