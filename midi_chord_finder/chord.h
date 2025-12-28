#include "note.h"

namespace ChordFinder {
#ifndef MAXNOTES
#define MAXNOTES 16
#endif // !MAXNOTES

#ifndef CHORDNAMES
#define CHORDNAMES
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


	const char* const seventh_names_no_fifth  [] = {
		// tertian chords
		"maj7 (5)",    // Major 7th
		"min7 (5)",    // Minor 7th
		"7 (5)",       // Dominant 7th
		"m7b5 (5)",    // Half-Diminished 7th
		"dim7 (5)",    // Diminished 7th
		"mmaj7 (5)",   // Minor-Major 7th
		"augmaj7 (5)", // Augmented-Major 7th
		//// non-tertian chords
		//"aug7 (5)",    // Augmented 7th
		//"mM7b5 (5)",   // Diminished-Major 7th
		//"7b5 (5)",     // Dominant 7th Flat Five
		//"M7b5 (5)"     // Major 7th Flat Five
	};
#endif // !CHORDNAMES

	class Chord
	{
	private:
		static bool _enabledSus2;

		char _notes_count;
		char _unique_notes_count;

		Note _notes[MAXNOTES];
		Note _unique_notes[MAXNOTES];

		bool _addNote(Note);
		bool _addNoteUnique(Note);

		bool _removeNote(Note);
		bool _removeNoteUnique(Note);

		const char* _name;
		Note* _root;

		void _resetName();
		static char _interval(char, char);
		void _determineChord();

		bool _tryFindTriad();
		bool _tryFind6th7th();
		bool _tryFindIncomplete7th();

	public:
		Chord();
		~Chord();

		bool AddNote(Note);
		bool RemoveNote(Note);
		void EnableSus2() { _enabledSus2 = true; }
		void DisableSus2() { _enabledSus2 = false; }

		const Note* GetNotes() const { return _notes; }
		char GetNotesCount() const { return _notes_count; }
		const char* GetName() const { return _name; }
		const char* GetRoot();
		char GetOctave();
	};
}