namespace ChordFinder {

#ifndef NOTENAMES
#define NOTENAMES
	static const char* const note_names[] = {
		"C", "C#", "D", "Eb", "E", "F",
		"F#", "G", "Ab", "A", "Bb", "B"
	};
#endif // !NOTENAMES

#ifndef NOTECLASS
#define NOTECLASS
	class Note
	{
	private:
		const char* _name;
		char _value;
		char _octave;
		static char _middleOctave;

	public:
		Note();
		Note(char);
		~Note() {};

		// Use this to set the middle octave (usually octave 3 or 4 depending on the context)
		// Midi note 60 is Middle C (C4), so default MiddleOctave is 4
		static bool SetMiddleOctave(char);

		static const char* GetNoteName(char value) { return note_names[value % 12]; }

		const char* GetName() const { return _name; }
		const char GetValue() const { return _value; }
		const char GetNormalizedValue() const { return _value % 12; }
		const char GetOctave() const { return _octave; }
	};
#endif // !NOTECLASS
}
