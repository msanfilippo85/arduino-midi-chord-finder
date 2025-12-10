#include "chords.h"

const char* const triad_names[] = {
	"sus2", // Suspended 2nd
	"dim",  // Diminished
	"min",  // Minor
	"maj",  // Major
	"aug",  // Augmented
	"sus4"  // Suspended 4th
};

const char* const sixth_names[] = {
    "min6", // Minor 6th
    "maj6", // Major 6th
};

const char* const seventh_names[] = {
	// tertian chords
	"maj7",    // Major 7th
	"min7",    // Minor 7th
	"7",       // Dominant 7th
	"m7b5",    // Half-Diminished 7th
	"dim7",    // Diminished 7th
	"mmaj7",   // Minor-Major 7th
	"augmaj7", // Augmented-Major 7th
	// non-tertian chords
	"aug7",    // Augmented 7th
	"mM7b5",   // Diminished-Major 7th
	"7b5",     // Dominant 7th Flat Five
	"M7b5"     // Major 7th Flat Five
};

static char interval(char root, char other)
{
    char diff = (other - root) % 12;
    if (diff < 0)
        diff += 12;
    return diff;
}

sChord get_chord_triad(sFullNote notes[])
{
	sChord default_chord = { -1, "", "" };
    for (char i = 0; i < 3; i++)
    {
        char root = notes[i].value;
        char third = notes[(i + 1) % 3].value;
        char fifth = notes[(i + 2) % 3].value;
        char intThird = interval(root, third);
        char intFifth = interval(root, fifth);
        if (ENABLE_SUS2 && intThird == 2 && intFifth == 7) // Suspended 2
        {
            sChord chord = { 
                3,
                get_note_name(root),
                triad_names[0]
            };
            return chord;
		}
        if (intThird == 3 && intFifth == 6) // Diminished
        { 
            sChord chord = { 
                3,
                get_note_name(root),
                triad_names[1]
            };
			return chord;
		}
        if (intThird == 3 && intFifth == 7) // Minor
        {
            sChord chord = {
                3,
                get_note_name(root),
                triad_names[2]
            };
            return chord;
        }
        if (intThird == 4 && intFifth == 7) // Major
        {
            sChord chord = {
                3,
                get_note_name(root),
                triad_names[3]
            };
            return chord;
		}
        if (intThird == 4 && intFifth == 8) // Augmented
        {
            sChord chord = {
                3,
                get_note_name(root),
                triad_names[4]
            };
            return chord;
        }
		if (intThird == 5 && intFifth == 7) // Suspended 4
        {
            sChord chord = {
                3,
                get_note_name(root),
                triad_names[5]
            };
            return chord;
        }
    }
	return default_chord; // No Chord Found
}

sChord get_chord_4notes(sFullNote notes[])
{
    sChord default_chord = { -1, "", "" };

    // Assume `notes` contains 4 pitch classes (possibly in any order).
    // For each note treated as root, build a presence map of intervals (mod 12)
    for (char i = 0; i < 4; i++) {
        char root = notes[i].value;
        char present[12] = {0};
        for (char j = 0; j < 4; j++) {
            if (j == i) continue;
            char iv = interval(root, notes[j].value);
            if (iv >= 0 && iv < 12) present[(int)iv] = 1;
        }

        if (present[3] && present[7] && present[9]) {
            sChord chord = {4, get_note_name(root), sixth_names[0]};
            return chord;
        }
        if (present[4] && present[7] && present[9]) {
            sChord chord = {4, get_note_name(root), sixth_names[1]};
            return chord;
        }

        // Check all seventh chord types in a reasonable priority order
        // 0: maj7 -> 4,7,11 /// https://en.wikipedia.org/wiki/Major_seventh_chord
        if (present[4] && present[7] && present[11]) {
            sChord chord = {4, get_note_name(root), seventh_names[0]};
            return chord;
        }
        // 1: min7 -> 3,7,10 /// https://en.wikipedia.org/wiki/Minor_seventh_chord
        if (present[3] && present[7] && present[10]) {
            sChord chord = {4, get_note_name(root), seventh_names[1]};
            return chord;
        }
        // 2: 7 (dominant) -> 4,7,10 /// https://en.wikipedia.org/wiki/Dominant_seventh_chord
        if (present[4] && present[7] && present[10]) {
            sChord chord = {4, get_note_name(root), seventh_names[2]};
            return chord;
        }
        // 3: m7b5 (half-diminished) -> 3,6,10
        if (present[3] && present[6] && present[10]) {
            sChord chord = {4, get_note_name(root), seventh_names[3]};
            return chord;
        }
        // 4: dim7 (fully diminished) -> 3,6,9
        if (present[3] && present[6] && present[9]) {
            sChord chord = {4, get_note_name(root), seventh_names[4]};
            return chord;
        }
        // 5: mmaj7 (minor-major) -> 3,7,11
        if (present[3] && present[7] && present[11]) {
            sChord chord = {4, get_note_name(root), seventh_names[5]};
            return chord;
        }
        // 6: augmaj7 (augmented major seventh) -> 4,8,11
        if (present[4] && present[8] && present[11]) {
            sChord chord = {4, get_note_name(root), seventh_names[6]};
            return chord;
        }
        // 7: aug7 (augmented dominant) -> 4,8,10
        if (present[4] && present[8] && present[10]) {
            sChord chord = {4, get_note_name(root), seventh_names[7]};
            return chord;
        }
        // 8: mM7b5 (diminished-major) -> 3,6,11
        if (present[3] && present[6] && present[11]) {
            sChord chord = {4, get_note_name(root), seventh_names[8]};
            return chord;
        }
        // 9: 7b5 (dominant b5) -> 4,6,10
        if (present[4] && present[6] && present[10]) {
            sChord chord = {4, get_note_name(root), seventh_names[9]};
            return chord;
        }
        // 10: M7b5 (major 7 b5) -> 4,6,11
        if (present[4] && present[6] && present[11]) {
            sChord chord = {4, get_note_name(root), seventh_names[10]};
            return chord;
        }
    }

    return default_chord; // No Chord Found
}

sChord get_chord(sFullNote notes[], char length)
{
	sChord default_chord = { -1, "", "" };
    if (length == 3)
    {
		return get_chord_triad(notes);
    }
    if (length == 4)
    {
        return get_chord_4notes(notes);
    }
    return default_chord;
}