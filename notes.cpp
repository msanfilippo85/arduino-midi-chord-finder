#pragma once
#include "notes.h"

const char* const note_names[] = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
};

const char* get_note_name(char note) {
    return note_names[note % 12];
}

sFullNote get_note_name_with_octave(char note) {
    sFullNote full_note;
	full_note.name = get_note_name(note);
	full_note.value = note;
    full_note.octave = (note / 12) - (MIDDLEOCTAVE - 3);
    return full_note;
}