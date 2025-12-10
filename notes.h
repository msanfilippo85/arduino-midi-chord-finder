#ifndef MIDDLEOCTAVE
// Define the middle octave (usually octave 3 or 4 depending on the context)
// Midi note 60 is Middle C (C4), so we define MIDDLEOCTAVE as 4
#define MIDDLEOCTAVE 4
#endif // !MIDDLEOCTAVE 

#ifndef NOTENAMESTRUCT
#define NOTENAMESTRUCT
typedef struct {
    const char* name;
    char value;
    char octave;
} sFullNote;
#endif // !NOTENAMESTRUCT

#ifndef NOTES_H
#define NOTES_H
const char* get_note_name(char);
sFullNote get_note_name_with_octave(char);
#endif // !NOTES_H
