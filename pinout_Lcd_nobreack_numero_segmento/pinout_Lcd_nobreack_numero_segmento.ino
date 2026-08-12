#include <Arduino.h>

/* ================= PINOUT confirmado ================= */
const uint8_t PIN_CS   = 2;  // CS activo LOW
const uint8_t PIN_WR   = 4;  // WR/CLK
const uint8_t PIN_DATA = 5;  // DATA MSB first
const uint8_t PIN_RST  = 3;  // RESET activo LOW

/* HT1621: 32 nibbles (0..31) confirmado */
const uint8_t RAM_NIBBLES = 32;

/* RAM sombra */
uint8_t shadow[RAM_NIBBLES];

/* ================= HT1621 low-level ================= */
static inline void dly() { delayMicroseconds(3); }
static inline void wrPulse() {
  digitalWrite(PIN_WR, HIGH); dly();
  digitalWrite(PIN_WR, LOW);  dly();
}

void htSendBits(uint32_t bits, uint8_t nbits) {
  for (int8_t i = (int8_t)nbits - 1; i >= 0; i--) {
    digitalWrite(PIN_DATA, (bits >> i) & 1);
    wrPulse();
  }
}

void htCommand(uint8_t cmd) {
  digitalWrite(PIN_CS, LOW);
  htSendBits(0b100, 3);
  htSendBits(cmd, 8);
  htSendBits(0, 1);
  digitalWrite(PIN_CS, HIGH);
}

void htWriteNibble(uint8_t addr, uint8_t nibble) {
  digitalWrite(PIN_CS, LOW);
  htSendBits(0b101, 3);
  htSendBits(addr & 0x3F, 6);
  htSendBits(nibble & 0x0F, 4);
  digitalWrite(PIN_CS, HIGH);
}

void htWriteAddr(uint8_t addr) {
  if (addr < RAM_NIBBLES) htWriteNibble(addr, shadow[addr]);
}

void htFlushAll() {
  for (uint8_t a = 0; a < RAM_NIBBLES; a++) htWriteNibble(a, shadow[a]);
}

void clearAll() {
  for (uint8_t a = 0; a < RAM_NIBBLES; a++) shadow[a] = 0;
  // Limpieza real en RAM
  for (uint8_t a = 0; a < RAM_NIBBLES; a++) htWriteNibble(a, 0x00);
}

void htInit() {
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_RST, OUTPUT);

  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_WR, LOW);
  digitalWrite(PIN_DATA, LOW);

  // RESET (activo LOW)
  digitalWrite(PIN_RST, HIGH); delay(5);
  digitalWrite(PIN_RST, LOW);  delay(5);
  digitalWrite(PIN_RST, HIGH); delay(5);

  // Oscilador interno
  htCommand(0x18); // RC256K

  // Init típica
  htCommand(0x01); // SYS EN
  htCommand(0x29); // BIAS 1/3, 4COM
  htCommand(0x03); // LCD ON
  htCommand(0x0A); // WDT DIS
  htCommand(0x08); // TONE OFF
}

/* ================= Control de bits ================= */
void setBit(uint8_t addr, uint8_t bit, bool on) {
  if (addr >= RAM_NIBBLES || bit > 3) return;
  uint8_t mask = (1 << bit);
  if (on) shadow[addr] |= mask;
  else    shadow[addr] &= ~mask;
  htWriteAddr(addr); // actualiza solo esa dirección
}

void toggleBit(uint8_t addr, uint8_t bit) {
  if (addr >= RAM_NIBBLES || bit > 3) return;
  shadow[addr] ^= (1 << bit);
  htWriteAddr(addr);
}

/* ================= 7 segmentos / 3 dígitos =================
   El usuario confirmó el orden REAL del dígito 1:
   0-1=E, 0-2=G, 0-3=F, 1-0=D, 1-1=C, 1-2=B, 1-3=A
   Por lo tanto en orden estándar A,B,C,D,E,F,G queda:
   A=(1,3) B=(1,2) C=(1,1) D=(1,0) E=(0,1) F=(0,3) G=(0,2)

   Se aplica el mismo patrón a dígito 2 (addr 2-3) y dígito 3 (addr 4-5).
*/

struct AB { uint8_t addr; uint8_t bit; };

// Orden estándar de segmentos: A,B,C,D,E,F,G
const AB DIG[3][7] = {
  // Dígito 1 (addr 0-1)
  { {1,3}, {1,2}, {1,1}, {1,0}, {0,1}, {0,3}, {0,2} },
  // Dígito 2 (addr 2-3)
  { {3,3}, {3,2}, {3,1}, {3,0}, {2,1}, {2,3}, {2,2} },
  // Dígito 3 (addr 4-5)
  { {5,3}, {5,2}, {5,1}, {5,0}, {4,1}, {4,3}, {4,2} }
};

// Máscaras estándar 7-seg (bit0=A ... bit6=G)
const uint8_t SEG_LUT[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

void setDigitMask(uint8_t digitIndex, uint8_t maskAtoG) {
  if (digitIndex > 2) return;
  for (uint8_t s = 0; s < 7; s++) {
    bool on = (maskAtoG >> s) & 1;
    setBit(DIG[digitIndex][s].addr, DIG[digitIndex][s].bit, on);
  }
}

void clearDigit(uint8_t digitIndex) {
  setDigitMask(digitIndex, 0x00);
}

// ===== Nueva función solicitada: mostrar número de 3 dígitos =====
// value: 0..999
// leadingZeros: true -> muestra 000..999, false -> oculta ceros a la izquierda
void displayNumber3(int value, bool leadingZeros = false) {
  if (value < 0) value = 0;
  if (value > 999) value = 999;

  int centenas = value / 100;
  int decenas  = (value / 10) % 10;
  int unidades = value % 10;

  if (leadingZeros) {
    setDigitMask(0, SEG_LUT[centenas]);
    setDigitMask(1, SEG_LUT[decenas]);
  } else {
    if (centenas == 0) {
      clearDigit(0);
      if (decenas == 0) clearDigit(1);
      else setDigitMask(1, SEG_LUT[decenas]);
    } else {
      setDigitMask(0, SEG_LUT[centenas]);
      setDigitMask(1, SEG_LUT[decenas]);
    }
  }

  setDigitMask(2, SEG_LUT[unidades]);
}

/* ================= Parser Serial ================= */
bool readLine(char *buf, size_t buflen) {
  static size_t idx = 0;
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { buf[idx] = 0; idx = 0; return true; }
    if (idx < buflen - 1) buf[idx++] = c;
  }
  return false;
}

void printHelp() {
  Serial.println("\nComandos:");
  Serial.println("  c                     -> limpia todo");
  Serial.println("  on <addr> <bit>        -> enciende bit (addr 0..31, bit 0..3)");
  Serial.println("  off <addr> <bit>       -> apaga bit");
  Serial.println("  tgl <addr> <bit>       -> alterna bit");
  Serial.println("  num <0..999>           -> muestra numero 3 digitos (sin ceros izq)");
  Serial.println("  numz <0..999>          -> muestra numero 3 digitos (con ceros izq)");
  Serial.println("  show                   -> reescribe (flush) todo shadow[]");
  Serial.println("Ej: num 123  |  numz 7  |  on 0 1");
}

void setup() {
  Serial.begin(115200);
  delay(300);

  htInit();
  clearAll();

  Serial.println("HT1621 listo.");
  printHelp();

  // Prueba inicial
  int n=0;
  for (int n=0 ;n <= 999; n++) {
  displayNumber3(n, false);
  delay (200);
  }
}

void loop() {
  static char line[50];
  if (!readLine(line, sizeof(line))) return;
  if (line[0] == 0) return;

  // c
  if ((line[0] == 'c' || line[0] == 'C') && line[1] == 0) {
    clearAll();
    Serial.println("LIMPIO.");
    return;
  }

  // show
  if (!strcmp(line, "show")) {
    htFlushAll();
    Serial.println("OK: show");
    return;
  }

  // help
  if (!strcmp(line, "help") || !strcmp(line, "?")) {
    printHelp();
    return;
  }

  // num / numz
  int n;
  if (sscanf(line, "numz %d", &n) == 1) {
    displayNumber3(n, true);
    Serial.print("OK numz "); Serial.println(n);
    return;
  }
  if (sscanf(line, "num %d", &n) == 1) {
    displayNumber3(n, false);
    Serial.print("OK num "); Serial.println(n);
    return;
  }

  // on/off/tgl addr bit
  char cmd[8] = {0};
  int addr = -1, bit = -1;
  if (sscanf(line, "%7s %d %d", cmd, &addr, &bit) == 3) {
    if (addr < 0 || addr >= RAM_NIBBLES || bit < 0 || bit > 3) {
      Serial.println("ERROR: rango (addr 0..31, bit 0..3)");
      return;
    }

    if (!strcmp(cmd, "on"))  { setBit((uint8_t)addr, (uint8_t)bit, true);  Serial.println("OK: on");  return; }
    if (!strcmp(cmd, "off")) { setBit((uint8_t)addr, (uint8_t)bit, false); Serial.println("OK: off"); return; }
    if (!strcmp(cmd, "tgl")) { toggleBit((uint8_t)addr, (uint8_t)bit);      Serial.println("OK: tgl"); return; }

    Serial.println("ERROR: comando (usa on/off/tgl)");
    return;
  }

  Serial.println("ERROR: comando no valido. Escribe 'help' para ver opciones.");
}
