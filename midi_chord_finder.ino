#include <MIDI.h>                // Library MIDI V5.0.2 Francois Best, lathoub
#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // Library LCD I2C v2.0.0 markub3327/LiquidCrystal_I2C
#include <string.h>

#include "notes.h"
#include "chords.h"

// Initialize MIDI on Serial
MIDI_CREATE_DEFAULT_INSTANCE();

#define LCD_ROWS 2
#define LCD_COLS 16

// Initialize LCD I2C (typical address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

#define MAXNOTES 8
sFullNote current_notes[MAXNOTES];
sFullNote current_notes_unique[MAXNOTES];
char current_note_count = 0;
char current_note_unique_count = 0;

char buffer[LCD_ROWS][LCD_COLS+1] = {0};

// Adds a note and keeps `current_notes` sorted by `value` in ascending order
sFullNote add_note_sort(char note) {
    sFullNote full_note = {NULL, -1, 0};
    if (current_note_count >= MAXNOTES) {
        return full_note;
    }

    full_note = get_note_name_with_octave(note);

    if (current_note_count == 0) {
        current_notes[0] = full_note;
        current_note_count = 1;
        return full_note;
    }

    // Find insertion index by scanning from the end (best for mostly-sorted arrays)
    int idx = current_note_count - 1;
    while (idx >= 0 && current_notes[idx].value > full_note.value) {
        // shift right
        current_notes[idx + 1] = current_notes[idx];
        idx--;
    }
    // insert at idx+1
    current_notes[idx + 1] = full_note;
    current_note_count++;

    return full_note;
}

// Adds a note to `current_notes_unique`, keeps array sorted by `value`, and
// does NOT insert if a note with the same pitch class (value mod 12) already exists.
sFullNote add_note_unique_sort(char note) {
    sFullNote full_note = {NULL, -1, 0};
    if (current_note_unique_count >= MAXNOTES) {
        return full_note;
    }

    full_note = get_note_name_with_octave(note);

    // Compute pitch class of incoming note (0..11)
    int new_pc = (int)full_note.value % 12;
    if (new_pc < 0) new_pc += 12;

    // Check for existing same pitch class
    for (int i = 0; i < (int)current_note_unique_count; ++i) {
        int existing_pc = (int)current_notes_unique[i].value % 12;
        if (existing_pc < 0) existing_pc += 12;
        if (existing_pc == new_pc) {
            // Same pitch class already present: do not insert
            return full_note;
        }
    }

    // Insert maintaining ascending order by `value`
    if (current_note_unique_count == 0) {
        current_notes_unique[0] = full_note;
        current_note_unique_count = 1;
        return full_note;
    }

    int idx = (int)current_note_unique_count - 1;
    while (idx >= 0 && current_notes_unique[idx].value > full_note.value) {
        current_notes_unique[idx + 1] = current_notes_unique[idx];
        idx--;
    }
    current_notes_unique[idx + 1] = full_note;
    current_note_unique_count++;

    return full_note;
}

// Removes a note from `current_notes_unique` by exact value and keeps array compact.
sFullNote remove_note_unique(char note) {
    sFullNote removed_note = { "--", -1, -1 };
    for (char i = 0; i < current_note_unique_count; i++) {
        if (current_notes_unique[i].value == note) {
            removed_note = current_notes_unique[i];
            // Shift remaining notes down
            for (char j = i; j < current_note_unique_count - 1; j++) {
                current_notes_unique[j] = current_notes_unique[j + 1];
            }
            current_note_unique_count--;
            break;
        }
    }
    return removed_note;
}

sFullNote remove_note(char note) {
    sFullNote removed_note = { "--", -1, -1 };
    for (char i = 0; i < current_note_count; i++) {
        if (current_notes[i].value == note) {
            removed_note = current_notes[i];
            // Shift remaining notes down
            for (char j = i; j < current_note_count - 1; j++) {
                current_notes[j] = current_notes[j + 1];
            }
            current_note_count--;
            break;
        }
    }
    return removed_note;
}

void display_current_notes_and_chord() {
    sprintf(buffer[0], "--");
    char pos = 0;
    for (char i = 0; i < current_note_count; i++) {

        char left = sizeof(buffer[0]) - pos;
        if(left == 0){
          break;
        }

        char written = (char)snprintf(buffer[0]+pos, left, "%s%d ", current_notes[i].name, current_notes[i].octave);

        if (written > 0) {
          if(written >= left){
            pos = sizeof(buffer[0]) - 1;
          }
          else {
            pos += written;
          }
        }
    }
    sChord chord = get_chord(current_notes_unique, current_note_unique_count);
    if (chord.notes >0) {
      snprintf(buffer[1], 16, "%s%s", chord.root, chord.name);
    } 
    else {
      sprintf(buffer[1], "--");
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buffer[0]);
    lcd.setCursor(0, 1);
    lcd.print(buffer[1]);
}


// --- Callback MIDI ---
void handleNoteOn(byte channel, byte note, byte velocity) {
    add_note_sort((char)note);
    add_note_unique_sort((char)note);
    display_current_notes_and_chord();
}

void handleNoteOff(byte channel, byte note, byte velocity) {
    remove_note((char)note);
    remove_note_unique((char)note);
    display_current_notes_and_chord();
}

void setup() {
  lcd.init();
  lcd.backlight();

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  lcd.setCursor(0, 0);
  lcd.print("--");
  lcd.setCursor(0, 1);
  lcd.print("Ready");
}

void loop() {
  MIDI.read();
}