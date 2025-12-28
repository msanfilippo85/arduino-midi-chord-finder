#include "chord.h"

bool ChordFinder::Chord::_enabledSus2 = false;

ChordFinder::Chord::Chord()
{
	_notes_count = 0;
	_unique_notes_count = 0;

	_root = nullptr;
	_resetName();
}

ChordFinder::Chord::~Chord() {}

bool ChordFinder::Chord::AddNote(Note note)
{
	return _addNote(note);
}

bool ChordFinder::Chord::RemoveNote(Note note)
{
	return _removeNote(note);
}

const char* ChordFinder::Chord::GetRoot()
{
	return _root == nullptr ? "--" : _root->GetName();
}

char ChordFinder::Chord::GetOctave()
{
	return _root == nullptr ? -99 : _root->GetOctave();
}

bool ChordFinder::Chord::_addNote(Note note)
{
	if (_notes_count < MAXNOTES)
	{
		// If empty, insert at start
		if (_notes_count == 0) {
			_notes[0] = note;
			_notes_count = 1;
			_addNoteUnique(note);
			return true;
		}

		// Find insertion index by scanning from the end (efficient if mostly sorted)
		int idx = (int)_notes_count - 1;
		while (idx >= 0 && _notes[idx].GetValue() > note.GetValue()) {
			// shift right
			_notes[idx + 1] = _notes[idx];
			idx--;
		}
		// insert at idx+1
		_notes[idx + 1] = note;
		_notes_count++;

		_addNoteUnique(note);
		_determineChord();
		return true;
	}
	return false;
}

bool ChordFinder::Chord::_removeNote(Note note)
{
	// Find exact match in _notes by absolute value and octave
	int found = -1;
	for (int i = 0; i < _notes_count; ++i) {
		if (_notes[i].GetValue() == note.GetValue() && _notes[i].GetOctave() == note.GetOctave()) {
			found = i;
			break;
		}
	}
	if (found < 0) return false;

	// Shift remaining notes left to remove
	for (int j = found; j < _notes_count - 1; ++j) {
		_notes[j] = _notes[j + 1];
	}
	_notes_count--;

	// Update unique notes accordingly
	_removeNoteUnique(note);
	_determineChord();
	return true;
}

bool ChordFinder::Chord::_addNoteUnique(Note note)
{
	int new_nv = (int)note.GetNormalizedValue();

	// If empty, just insert
	if (_unique_notes_count == 0) {
		_unique_notes[0] = note;
		_unique_notes_count = 1;
		return true;
	}

	// Find insertion index and check for duplicates
	int insert_idx = _unique_notes_count; // default append
	for (int i = 0; i < _unique_notes_count; ++i) {
		int existing_nv = (int)_unique_notes[i].GetNormalizedValue();
		if (existing_nv == new_nv) {
			// duplicate normalized value: keep note with greater octave
			if (_unique_notes[i].GetOctave() < note.GetOctave()) {
				_unique_notes[i] = note;
				return true; // replaced
			}
			return false; // existing equal or higher octave, do not insert
		}
		if (existing_nv > new_nv) {
			insert_idx = i;
			break;
		}
	}

	// Insert at insert_idx by shifting right
	if (_unique_notes_count < MAXNOTES) {
		for (int k = _unique_notes_count; k > insert_idx; --k) {
			_unique_notes[k] = _unique_notes[k - 1];
		}
		_unique_notes[insert_idx] = note;
		_unique_notes_count++;
		return true;
	}

	return false;
}

bool ChordFinder::Chord::_removeNoteUnique(Note note)
{
	int nv = (int)note.GetNormalizedValue();
	// Find index in unique notes
	int idx = -1;
	for (int i = 0; i < _unique_notes_count; ++i) {
		if ((int)_unique_notes[i].GetNormalizedValue() == nv) {
			idx = i;
			break;
		}
	}
	if (idx < 0) return false; // nothing to remove

	// Look for another note in _notes with same normalized value and highest octave
	int bestIndex = -1;
	int bestOctave = -128;
	for (int i = 0; i < _notes_count; ++i) {
		if ((int)_notes[i].GetNormalizedValue() == nv) {
			int oct = (int)_notes[i].GetOctave();
			if (oct > bestOctave) {
				bestOctave = oct;
				bestIndex = i;
			}
		}
	}

	if (bestIndex >= 0) {
		// Replace unique entry with the highest-octave remaining note
		_unique_notes[idx] = _notes[bestIndex];
		return true;
	}

	// No remaining note with that normalized value: remove unique entry
	for (int k = idx; k < _unique_notes_count - 1; ++k) {
		_unique_notes[k] = _unique_notes[k + 1];
	}
	_unique_notes_count--;
	return true;
}

void ChordFinder::Chord::_resetName()
{
	_name = "--";
}

char ChordFinder::Chord::_interval(char root, char other)
{
	char diff = (other - root) % 12;
	if (diff < 0)
		diff += 12;
	return diff;
}

void ChordFinder::Chord::_determineChord()
{
	_root = nullptr;
	_resetName();
	if(_unique_notes_count == 3)
	{
		if (!_tryFindTriad())
		{
			_tryFindIncomplete7th();
		}
	}
	else if (_unique_notes_count == 4)
	{
		_tryFind6th7th();
	}
}

bool ChordFinder::Chord::_tryFindTriad()
{
	for (char i = 0; i < 3; i++)
	{
		char root = _unique_notes[i].GetNormalizedValue();
		char third = _unique_notes[(i + 1) % 3].GetNormalizedValue();
		char fifth = _unique_notes[(i + 2) % 3].GetNormalizedValue();
		char intThird = _interval(root, third);
		char intFifth = _interval(root, fifth);
		if (_enabledSus2 && intThird == 2 && intFifth == 7) // Suspended 2
		{
			_root = &_unique_notes[i];
			_name = triad_names[0];
			return true;
		}
		if (intThird == 3 && intFifth == 6) // Diminished
		{
			_root = &_unique_notes[i];
			_name = triad_names[1];
			return true;
		}
		if (intThird == 3 && intFifth == 7) // Minor
		{
			_root = &_unique_notes[i];
			_name = triad_names[2];
			return true;
		}
		if (intThird == 4 && intFifth == 7) // Major
		{
			_root = &_unique_notes[i];
			_name = triad_names[3];
			return true;
		}
		if (intThird == 4 && intFifth == 8) // Augmented
		{
			_root = &_unique_notes[i];
			_name = triad_names[4];
			return true;
		}
		if (intThird == 5 && intFifth == 7) // Suspended 4
		{
			_root = &_unique_notes[i];
			_name = triad_names[5];
			return true;
		}
	}
	return false; // No Chord Found
}

bool ChordFinder::Chord::_tryFind6th7th()
{
		for (char i = 0; i < 4; i++) {
			char root = _unique_notes[i].GetNormalizedValue();
			char present[12] = { 0 };
			for (char j = 0; j < 4; j++) {
				if (j == i) continue;
				char iv = _interval(root, _unique_notes[j].GetNormalizedValue());
				if (iv >= 0 && iv < 12) present[(int)iv] = 1;
			}

			// Check for 6th chords
			if (present[3] && present[7] && present[9]) {
				_root = &_unique_notes[i];
				_name = sixth_names[0];
				return true;
			}
			if (present[4] && present[7] && present[9]) {
				_root = &_unique_notes[i];
				_name = sixth_names[1];
				return true;
			}

	//		// Check all seventh chord types in a reasonable priority order
	//		// 0: maj7 -> 4,7,11 /// https://en.wikipedia.org/wiki/Major_seventh_chord
			if (present[4] && present[7] && present[11]) {
				_root = &_unique_notes[i];
				_name = seventh_names[0];
				return true;
			}
			// 1: min7 -> 3,7,10 /// https://en.wikipedia.org/wiki/Minor_seventh_chord
			if (present[3] && present[7] && present[10]) {
				_root = &_unique_notes[i];
				_name = seventh_names[1];
				return true;
			}
			// 2: 7 (dominant) -> 4,7,10 /// https://en.wikipedia.org/wiki/Dominant_seventh_chord
			if (present[4] && present[7] && present[10]) {
				_root = &_unique_notes[i];
				_name = seventh_names[2];
				return true;
			}
			// 3: m7b5 (half-diminished) -> 3,6,10 /// https://en.wikipedia.org/wiki/Half-diminished_seventh_chord
			if (present[3] && present[6] && present[10]) {
				_root = &_unique_notes[i];
				_name = seventh_names[3];
				return true;
			}
			// 4: dim7 (fully diminished) -> 3,6,9 /// https://en.wikipedia.org/wiki/Diminished_seventh_chord
			if (present[3] && present[6] && present[9]) {
				_root = &_unique_notes[i];
				_name = seventh_names[4];
				return true;
			}
			// 5: mmaj7 (minor-major) -> 3,7,11 /// https://en.wikipedia.org/wiki/Minor-major_seventh_chord
			if (present[3] && present[7] && present[11]) {
				_root = &_unique_notes[i];
				_name = seventh_names[5];
				return true;
			}
			// 6: augmaj7 (augmented major seventh) -> 4,8,11 /// https://en.wikipedia.org/wiki/Augmented_major_seventh_chord
			if (present[4] && present[8] && present[11]) {
				_root = &_unique_notes[i];
				_name = seventh_names[6];
				return true;
			}
			// 7: aug7 (augmented dominant) -> 4,8,10 /// https://en.wikipedia.org/wiki/Augmented_seventh_chord
			if (present[4] && present[8] && present[10]) {
				_root = &_unique_notes[i];
				_name = seventh_names[7];
				return true;
			}
			// 8: mM7b5 (diminished-major) -> 3,6,11 /// https://en.wikipedia.org/wiki/Diminished-major_seventh_chord
			if (present[3] && present[6] && present[11]) {
				_root = &_unique_notes[i];
				_name = seventh_names[8];
				return true;
			}
			// 9: 7b5 (dominant b5) -> 4,6,10 /// https://en.wikipedia.org/wiki/Dominant_seventh_flat_five_chord
			if (present[4] && present[6] && present[10]) {
				_root = &_unique_notes[i];
				_name = seventh_names[9];
				return true;
			}
			// 10: M7b5 (major 7 b5) -> 4,6,11
			if (present[4] && present[6] && present[11]) {
				_root = &_unique_notes[i];
				_name = seventh_names[10];
				return true;
			}
		}
	return false;
}

bool ChordFinder::Chord::_tryFindIncomplete7th()
{
	// Expecting exactly 3 unique notes when called
	if (_unique_notes_count != 3) return false;

	// For each note as possible root
	for (char i = 0; i < 3; ++i)
	{
		char root = _unique_notes[i].GetNormalizedValue();
		char present[12] = { 0 };
		for (char j = 0; j < 3; ++j)
		{
			if (j == i) continue;
			char iv = _interval(root, _unique_notes[j].GetNormalizedValue());
			if (iv >= 0 && iv < 12) present[(int)iv] = 1;
		}

		// We are looking for seventh-type chords where the perfect fifth (7) is missing
		if (present[7]) continue; // has perfect fifth -> not an "incomplete" missing fifth

		// Check candidate seventh chord patterns in a reasonable priority order.
		// Each pattern is {nameIndex, thirdInterval, seventhInterval}
		const struct { char nameIndex; char third; char seventh; } patterns[] = {
			{ 0, 4, 11 }, // maj7
			{ 1, 3, 10 }, // min7
			{ 2, 4, 10 }, // 7 (dominant)
			{ 3, 3, 10 }, // m7b5
			{ 4, 3, 9  }, // dim7
			{ 5, 3, 11 }, // mmaj7
			{ 6, 4, 11 }, // augmaj7
			//{ 7, 4, 10 }, // aug7
			//{ 8, 3, 11 }, // mM7b5
			//{ 9, 4, 10 }, // 7b5
			//{ 10,4, 11 }  // M7b5
		};

		for (char p = 0; p < sizeof(patterns)/sizeof(patterns[0]); ++p)
		{
			char t = patterns[p].third;
			char s = patterns[p].seventh;
			if (present[t] && present[s])
			{
				_root = &_unique_notes[i];
				_name = seventh_names_no_fifth[patterns[p].nameIndex];
				return true;
			}
		}
	}

	return false;
}
