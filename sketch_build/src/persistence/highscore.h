#ifndef HIGHSCORE_H
#define HIGHSCORE_H
#include <EEPROM.h>

// Two-slot wear-leveling high-score storage
// Slot layout (8 bytes each):
//   [0] = magic byte (0xA5)
//   [1] = sequence number (wraps 0-255; higher = more recent)
//   [2..5] = top 4 scores (byte each, max 99)
//   [6] = reserved
//   [7] = CRC8 of bytes [0..6]

#define HS_MAGIC        0xA5
#define HS_SLOT_SIZE    8
#define HS_SLOT0_ADDR   0
#define HS_SLOT1_ADDR   8
#define HS_NUM_SCORES   4
#define HS_WRITE_MIN_INTERVAL_MS 1000UL  // rate-limit writes

static byte _hsScores[HS_NUM_SCORES];
static byte _hsSlot = 0;        // which slot is active
static byte _hsSeq  = 0;        // sequence number of active slot
static unsigned long _hsLastWrite = 0;

static byte _hsCRC8(const byte* data, byte len) {
  byte crc = 0;
  for (byte i = 0; i < len; i++) {
    crc ^= data[i];
    for (byte j = 0; j < 8; j++) crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
  }
  return crc;
}

static bool _hsReadSlot(byte slot, byte* seq_out, byte scores[HS_NUM_SCORES]) {
  int addr = (slot == 0) ? HS_SLOT0_ADDR : HS_SLOT1_ADDR;
  byte buf[HS_SLOT_SIZE];
  for (byte i = 0; i < HS_SLOT_SIZE; i++) buf[i] = EEPROM.read(addr + i);
  // Magic-byte check first: fast rejection of uninitialised/erased slots (0xFF).
  // CRC-8 then covers the remaining data bytes for corruption detection.
  if (buf[0] != HS_MAGIC) return false;
  byte crc = _hsCRC8(buf, HS_SLOT_SIZE - 1);
  if (crc != buf[HS_SLOT_SIZE - 1]) return false;
  *seq_out = buf[1];
  for (byte i = 0; i < HS_NUM_SCORES; i++) scores[i] = buf[2 + i];
  return true;
}

static void _hsWriteSlot(byte slot, byte seq, const byte scores[HS_NUM_SCORES]) {
  int addr = (slot == 0) ? HS_SLOT0_ADDR : HS_SLOT1_ADDR;
  byte buf[HS_SLOT_SIZE];
  buf[0] = HS_MAGIC;
  buf[1] = seq;
  for (byte i = 0; i < HS_NUM_SCORES; i++) buf[2 + i] = scores[i];
  buf[6] = 0;
  buf[7] = _hsCRC8(buf, HS_SLOT_SIZE - 1);
  for (byte i = 0; i < HS_SLOT_SIZE; i++) {
    if (EEPROM.read(addr + i) != buf[i]) EEPROM.write(addr + i, buf[i]);
  }
}

// Load high scores from EEPROM. Logs state via Serial if Serial is begun.
inline void loadHighScores() {
  byte seq0 = 0, seq1 = 0;
  byte scores0[HS_NUM_SCORES] = {0}, scores1[HS_NUM_SCORES] = {0};
  bool ok0 = _hsReadSlot(0, &seq0, scores0);
  bool ok1 = _hsReadSlot(1, &seq1, scores1);

  if (!ok0 && !ok1) {
    for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = 0;
    _hsSlot = 0; _hsSeq = 0;
    Serial.println(F("HS: no valid EEPROM data, fresh start"));
  } else if (ok0 && !ok1) {
    for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = scores0[i];
    _hsSlot = 0; _hsSeq = seq0;
    Serial.print(F("HS: slot 0 valid, seq=")); Serial.println(seq0);
  } else if (!ok0 && ok1) {
    for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = scores1[i];
    _hsSlot = 1; _hsSeq = seq1;
    Serial.print(F("HS: slot 1 valid, seq=")); Serial.println(seq1);
  } else {
    // Both valid — pick most recent by sequence number (wrapping-safe via unsigned subtraction)
    // diff = seq0 - seq1: if diff < 128, seq0 is ahead (more recent); otherwise seq1 is more recent.
    byte diff = seq0 - seq1;
    if (diff < 128) {
      for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = scores0[i];
      _hsSlot = 0; _hsSeq = seq0;
      Serial.print(F("HS: both valid, slot 0 newer, seq=")); Serial.println(seq0);
    } else {
      for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = scores1[i];
      _hsSlot = 1; _hsSeq = seq1;
      Serial.print(F("HS: both valid, slot 1 newer, seq=")); Serial.println(seq1);
    }
  }
}

// Save a new score if it beats an existing slot; applies wear-leveling.
inline void saveHighScore(byte newScore) {
  if (newScore == 0) return;
  // Rate-limit writes. The subtraction is wrap-safe for unsigned long arithmetic:
  // when millis() overflows (~49.7 days), the difference still gives the correct
  // elapsed time modulo 2^32.
  if (_hsLastWrite != 0 && millis() - _hsLastWrite < HS_WRITE_MIN_INTERVAL_MS) return;

  bool improved = false;
  for (byte i = 0; i < HS_NUM_SCORES; i++) {
    if (newScore > _hsScores[i]) { improved = true; break; }
  }
  if (!improved) return;

  // Insert into sorted list (descending)
  for (byte i = 0; i < HS_NUM_SCORES; i++) {
    if (newScore > _hsScores[i]) {
      for (byte j = HS_NUM_SCORES - 1; j > i; j--) _hsScores[j] = _hsScores[j - 1];
      _hsScores[i] = newScore;
      break;
    }
  }

  // Write to the alternate slot for wear-leveling
  byte nextSlot = (_hsSlot == 0) ? 1 : 0;
  _hsSeq++;
  _hsWriteSlot(nextSlot, _hsSeq, _hsScores);
  _hsSlot = nextSlot;
  _hsLastWrite = millis();
}

inline void clearHighScores() {
  for (byte i = 0; i < HS_NUM_SCORES; i++) _hsScores[i] = 0;
  _hsSlot = 0; _hsSeq = 0;
  // Invalidate both slots by overwriting the magic byte
  EEPROM.write(HS_SLOT0_ADDR, 0x00);
  EEPROM.write(HS_SLOT1_ADDR, 0x00);
}

inline byte         getHighScore()  { return _hsScores[0]; }
inline const byte*  getHighScores() { return _hsScores; }

#endif // HIGHSCORE_H 
