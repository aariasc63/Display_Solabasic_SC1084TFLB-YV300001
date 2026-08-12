#include <Arduino.h>

/* ===== PINOUT confirmado ===== */
const uint8_t PIN_CS   = 2;  // CS (activo LOW)
const uint8_t PIN_WR   = 4;  // WR/CLK
const uint8_t PIN_DATA = 5;  // DATA (MSB first)
const uint8_t PIN_X    = 3;  // EXTRA: posible RESET/RD (lo dejamos en estado seguro)

/* ===== Timings ===== */
static inline void dly() { delayMicroseconds(3); }

static inline void wrPulse() {
  digitalWrite(PIN_WR, HIGH); dly();
  digitalWrite(PIN_WR, LOW);  dly();
}

/* ===== Envío MSB-first ===== */
void htSendBits(uint32_t bits, uint8_t nbits) {
  for (int8_t i = (int8_t)nbits - 1; i >= 0; i--) {
    digitalWrite(PIN_DATA, (bits >> i) & 1);
    wrPulse();
  }
}

/* ===== Comando: 100 + cmd(8) + 0 ===== */
void htCommand(uint8_t cmd) {
  digitalWrite(PIN_CS, LOW);
  htSendBits(0b100, 3);
  htSendBits(cmd, 8);
  htSendBits(0, 1);
  digitalWrite(PIN_CS, HIGH);
}

/* ===== Write RAM: 101 + addr(6) + data(4) ===== */
void htWriteNibble(uint8_t addr, uint8_t nibble) {
  digitalWrite(PIN_CS, LOW);
  htSendBits(0b101, 3);
  htSendBits(addr & 0x3F, 6);
  htSendBits(nibble & 0x0F, 4);
  digitalWrite(PIN_CS, HIGH);
}

/* ===== Escribe muchas direcciones para “cubrir” el LCD ===== */
void htFill(uint8_t nibble) {
  for (uint8_t a = 0; a < 64; a++) {
    htWriteNibble(a, nibble);
  }
}

void htInit() {
  // estados base
  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_WR, LOW);
  digitalWrite(PIN_DATA, LOW);

  // Si PIN_X es RESET (no siempre), damos un pulso suave
  digitalWrite(PIN_X, LOW);  delay(2);
  digitalWrite(PIN_X, HIGH); delay(2);
  digitalWrite(PIN_X, LOW);  delay(2);

  // Init típica HT1621
  htCommand(0x01); // SYS EN
  htCommand(0x29); // BIAS 1/3, 4COM (tu módulo respondió aquí)
  htCommand(0x03); // LCD ON
  htCommand(0x0A); // WDT DIS
  htCommand(0x08); // TONE OFF
}

void setup() {
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_X, OUTPUT);

  htInit();

  // Prueba visible
  htFill(0x0F);  // todo ON
  delay(1200);
  htFill(0x00);  // todo OFF
}

void loop() {
  // Parpadeo continuo para confirmar funcionamiento
  htFill(0x0F);
  delay(800);
  htFill(0x00);
  delay(800);
}
