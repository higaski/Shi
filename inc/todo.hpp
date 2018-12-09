/// \page page_todo TODO
/// -# create sollte von Anfang an vollwertige Einträge erzeugen. Das war beim
///    mecrisp ja nur limitierend weil die gleich ins Flash compilieren und ned
///    zerst ins RAM. Das Problem is, dass momentan so Dinge wie "17 create
///    seventeen ," nicht funktionieren, obwohl das ANS is. Dummerweise wird
///    ohne diesem "end:;" was ich aus mecrisp übernommen hab weder Links noch
///    Flags geschrieben... und alles was ohne :; Päärchen erzeugt wird hängt in
///    da Luft.<br>
///    Wenn ich so drüber nachdenk wärs vielleicht sinnvoll nur :; Definitionen
///    im Flash zuzulassen?<br>
///    Ooooder! Man ruft NACH einer Definition ein spezielles Word auf, dass die
///    letzt erzeugte Definition ins Flash schreibt? Das is vielleicht noch
///    besser? Dann spart ma sich auch gleich den Schaß wo ständig geprüft wern
///    muss wo grad hingeschrieben wird? Es wird einfach IMMER ins RAM
///    compiliert und wenn dieses spezielle Wort aufgerufen wird, dann wird der
///    Link angepasst und die ganze Definition ins Flash kopiert. Fertig?<br>
///    Aktuell kommt "comma_q", sprich die Überprüfung ob RAM/FLASH übrigens an
///    2x Stellen vor:
///    - compile,
///    - end:;
///
///    Haken an der Sache is, dass ma dann vermutlich nur noch absolut und nimma
///    relativ Branchen können? Und des is... Oasch. Dann kost nämlich jeder
///    Sprung plötzlich 3x4 statt 4 Byte -.-
///
///    Ok, na, das is mega Oasch. Fuck! Wenn ma muss sich VORHER aussuchen dass
///    die Definition im FLASH landet, aber dann sind ma quasi erst wieder da wo
///    ma jetzt sind unds an Schalter gibt wo ma hin und her schalten kann.
///
/// -# In Gforth gibts zum Beispiel "latestxt" um es letzte xt das definiert
///    wurde zu bekommen.
///
/// -# does> fehlt no und is SAU kompliziert...
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
/// \page page_doc_stuff Doc stuff
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
/// \page page_doc_stuff Doc stuff
///
/// Bezüglich Nummern...<br>
/// HEX wird AUSSCHLIESSLICH!!!! in Großbuchstaben unterstützt!!!
///
/// Fold Flags gibts nur für Words, die pure sind, sprich keine Nebeneffekte
/// haben. Die Anzahl der Eingangsparameter is fürs Falten wichtig. Sprich 1x
/// Eingangsparameter -> FOLDS_1, 2x Eingangsparameter -> FOLDS_2 usw.
