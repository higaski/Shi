/// \page page_todo TODO
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
/// Die Suche nach Eintraegen beginnt IMMER mit "l_mem". l_mem wird auch nur
/// geupdatet wenn ein Eintrag ins RAM gschrieben wird. l_mem is somit quasi
/// immer des Ende vom Faden wo ma zu Suchen beginnt.
///
/// Was wuerde eine externe Flash-Write Funktion brauchen?
/// - Pointer zum Beginn der Definition im RAM
/// - Laenge der Definition im RAM
/// - Flags fuer die Definition
/// - Link der letzten Definition
/// Und als return wuerde sich wohl ein Update von p_mem_flash anbieten?
///
/// Folgende Reihenfolge beim Flash-Write aktuell:
/// -# Laenge der Definition wird aligned
/// -# Jene Laenge wird zu p_mem_ram dazu addiert und wieder gespeichert (in
///    p_mem_ram+4 steht der Beginn der aktuellen Definition der waehrend
///    "create" gesetzt wird!) und an jener Stelle muss I anmerken, des verdient
///    a eigenes Symbol!!!
/// -# Link und Flags werden noch ins RAM geschrieben
/// -# Dann folgt der elends lange Flash Schaß
/// -# Dann wird bis zum Beginn der aktuellen Definition des RAM wieder geleert
///    (0xFF)
///
// clang-format off
/// \page page_todo TODO
/// | Symbols         | Responsibility                                                                     |
/// | ----------------| ---------------------------------------------------------------------------------- |
/// | _s_shi_dstack   | Symbol at start of stack                                                           |
/// | _e_shi_dstack   | Symbol at end of stack                                                             |
/// | _s_shi_context  | Symbol at start of context (tos, dsp and lfp)                                      |
/// | _s_shi_context  | Symbol at end of context                                                           |
/// | p_mem_ram       | Pointer to ram (and beginning of current definition)                               |
/// | p_mem_flash     | Pointer to flash                                                                   |
/// | p_variable      | Used for reserving ram for variables                                               |
/// | p_structure     | Inside loop: points to leave addresses from the current loop on the stack <br><!-- |
/// |                 | --> Inside case: points to end of addresses from the current case on the stack     |
/// | l_mem           | Contains address of link of the last definition                                    |
/// | status          | Current state (state is taken as word) <br><!--                                    |
/// |                 | --> false: interpret, true: compile                                                |
/// | status_compiler | Current compiler state <br><!--                                                    |
/// |                 | --> false: compile to ram, true: compile to flash                                  |
/// | src             | Source (address and length)                                                        |
/// | in              | Index in terminal input buffer                                                     |
/// | radix           | Determine current numerical base (base is taken as word)                           |
// clang-format on
/// \page page_todo TODO
