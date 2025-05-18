#include "config.h"
void play_buzzer() {
  for (int i = 0; i < n_notes; i++) {
    tone(BUZZER, notes[i], 200);
    delay(250);
    noTone(BUZZER);
    delay(50);
  }
}
