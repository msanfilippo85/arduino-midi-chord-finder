#include "notes.h"

#ifndef ENABLE_SUS2
#define ENABLE_SUS2 0 // settare a 1 per abilitare il rilevamento degli accordi sus2
#endif // !ENABLE_SUS2

#ifndef CHORDSTRUCT
#define CHORDSTRUCT
typedef struct {
	const char notes;
	const char* root;
	const char* name;
} sChord;
#endif // !CHORDSTRUCT

sChord get_chord(sFullNote[], char);