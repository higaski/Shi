/// \page page_todo TODO
/// RAM und FLASH Bereiche sollen vom User festgelegt wern
///
/// Die Funktion "shi_set_memory_space_pointer_flash" wird aber weiterhin
/// gebraucht. Weil wenn da User neue Funktionen im Flash anlegt, dann muessen
/// ma es user Flash durchsuchen und schaun ab wo frei is...
///
/// Der ganze Scheiss is so kompliziert...
///
/// Es gibt im Prinzip 3x Speicherbereiche:
/// - Core dict im flash
/// - User dict im ram
/// - User dict im flash
///
/// Des core dict wird von vorne nach hinten, ebenso das user dict im flash. Der
/// letzte Eintrag im core dict zeigt (momentan) auf den user flash bereich. Je
/// nachdem ob der leer is oder dort scho was steht gehts halt munter weiter...
///
/// Des user dict im ram wird von hinten nach vorne durchsucht. Das heißt die
/// Reihenfolge is:
/// - user dict ram -> core dict flash -> user dict flash
/// Die Suche nach Eintraegen beginnt IMMER mit "l_mem". l_mem wird auch nur
/// geupdatet wenn ein Eintrag ins RAM gschrieben wird. l_mem is somit quasi
/// immer des Ende vom Faden wo ma zu Suchen beginnt.
///
/// Die Frage is jetzt... wie zur hoelle wird ma des "TAIL" word los? Da des
/// Ding im Flash steht kann letzte Eintrag aktuell nur hardcoded sein...
///
// clang-format off
/// \page page_todo TODO
/// The main goal should be to get rid of ANY processor specific stuff
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
/// |                 | --> Inside case: points to endof addresses from the current case on the stack      |
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
