#include "note.h"

char ChordFinder::Note::_middleOctave = 4;

ChordFinder::Note::Note(){
	_value = -1;
	_octave = 0;
	_name = "--";
}

ChordFinder::Note::Note(char value)
{
	char baseNote = (value / 12) * 12;
	char offset = (baseNote - 60) / 12;
	_name = Note::GetNoteName(value);
	_value = value;
	_octave = _middleOctave + offset;
}

bool ChordFinder::Note::SetMiddleOctave(char middleOctave)
{
	if (middleOctave > 2 && middleOctave < 6)
	{
		_middleOctave = middleOctave;
		return 1;
	}
	return 0;
}
