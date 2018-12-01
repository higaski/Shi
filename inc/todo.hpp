/// \page page_todo TODO
///
/// Data-stack -> Selbst gemanaged via tos und dsp, normal im RAM<br>
/// Return-stack -> ECHTER "C" stack managed via sp... (so DONT FUCK THAT UP)
///
/// RAM und FLASH Bereiche sollen vom User festgelegt wern
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
/// Bereich genommen! Und nacher wird ram_end entsprechend angepasst.
///
/// csp<br>
/// csp is im Prinzip ein Stackpointer... Im Gegensatz zum normalen Stackpointer
/// (dsp) wächst der aber nicht nach unten in den Stack sondern fängt unten an
/// und wächst nach oben. Von der Richtung her wird "Struktur" Info am Stack
/// abgelegt, wie mas zum Beispiel für "leave" (break aus loop) oder "endcase"
/// (break aus switch-case) braucht
///
// clang-format off
/// \page page_todo TODO
/// | Symbols         | Responsibility                                                                     |
/// | ----------------| ---------------------------------------------------------------------------------- |
/// | _s_shi_dstack   | Symbol at start of stack                                                           |
/// | _e_shi_dstack   | Symbol at end of stack                                                             |
/// | _s_shi_context  | Symbol at start of context (tos, dsp and lfp)                                      |
/// | _s_shi_context  | Symbol at end of context                                                           |
/// | ram_begin       | Pointer to ram (and beginning of current definition)                               |
/// | flash_begin     | Pointer to flash                                                                   |
/// | ram_end         | Used for reserving ram for variables                                               |
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
/// \page page_todo TODO
///
