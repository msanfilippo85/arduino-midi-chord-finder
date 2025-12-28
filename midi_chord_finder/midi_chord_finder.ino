#include <MIDI.h>  // Library MIDI V5.0.2 Francois Best, lathoub
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Libreria LCD I2C v2.0.0 markub3327/LiquidCrystal_I2C
#include <string.h>

#include "note.h"
#include "chord.h"

// Inizializza MIDI su Serial
MIDI_CREATE_DEFAULT_INSTANCE();

#define LCD_ROWS 2
#define LCD_COLS 16

// Inizializza LCD I2C (indirizzo tipico 0x27, 16 colonne, 2 righe)
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

char buffer[LCD_ROWS][LCD_COLS + 1] = { 0 };

using namespace ChordFinder;
ChordFinder::Chord chord;

void display_current_notes_and_chord() {
  snprintf(buffer[0], 3, "--");
  char pos = 0;
  for (char i = 0; i < chord.GetNotesCount(); i++) {

    char left = sizeof(buffer[0]) - pos;
    if (left == 0) {
      break;
    }

    char written = (char)snprintf(buffer[0] + pos, left, "%s%d ", chord.GetNotes()[i].GetName(), chord.GetNotes()[i].GetOctave());

    if (written > 0) {
      if (written >= left) {
        pos = sizeof(buffer[0]) - 1;
      } else {
        pos += written;
      }
    }
  }
  if (chord.GetOctave() != -99) {
    // snprintf(buffer[1], 16, "%s[%d]%s", chord.GetRoot(), chord.GetOctave(), chord.GetName());
    snprintf(buffer[1], 16, "%s%s", chord.GetRoot(), chord.GetName());
  } else {
    snprintf(buffer[1], 3, "--");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(buffer[0]);
  lcd.setCursor(0, 1);
  lcd.print(buffer[1]);
}


// --- Callback MIDI ---
void handleNoteOn(byte channel, byte note, byte velocity) {
  ChordFinder::Note _note((char)note);
  chord.AddNote(_note);
  display_current_notes_and_chord();
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  ChordFinder::Note _note((char)note);
  chord.RemoveNote(_note);
  display_current_notes_and_chord();
}

void setup() {
  lcd.init();
  lcd.backlight();

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  lcd.setCursor(0, 0);
  lcd.print("Chord Finder 1.0");

  ChordFinder::Note::SetMiddleOctave(3);
}

void loop() {
  MIDI.read();
}