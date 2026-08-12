#include <SolabasicHT1621.h>

// Pines predeterminados:
// CS=D2, WR/CLK=D4, DATA=D5, RESET=D3.
SolabasicHT1621 lcd;

// Todos los iconos que ya fueron identificados en el LCD.
const SolabasicHT1621::Icon ICONOS[] = {
  SolabasicHT1621::ICON_OUTPUT,
  SolabasicHT1621::ICON_LOAD,
  SolabasicHT1621::ICON_FAULT,
  SolabasicHT1621::ICON_VCA2,
  SolabasicHT1621::ICON_HZ2,
  SolabasicHT1621::ICON_PERCENT,
  SolabasicHT1621::ICON_LOWER_FRAME,
  SolabasicHT1621::ICON_VCA1,
  SolabasicHT1621::ICON_HZ1,
  SolabasicHT1621::ICON_VCC1,
  SolabasicHT1621::ICON_BACKUP_MODE,
  SolabasicHT1621::ICON_BALANCE,
  SolabasicHT1621::ICON_BATTERY_FULL,
  SolabasicHT1621::ICON_BATTERY_PM,
  SolabasicHT1621::ICON_LINE_MODE,
  SolabasicHT1621::ICON_AVR,
  SolabasicHT1621::ICON_AC,
  SolabasicHT1621::ICON_DECIMAL_1,
  SolabasicHT1621::ICON_BATTERY,
  SolabasicHT1621::ICON_INPUT
};

const uint8_t CANTIDAD_ICONOS = sizeof(ICONOS) / sizeof(ICONOS[0]);

void probarDisplaysNumericos() {
  lcd.clear();

  // Display 1: INPUT / BATTERY.
  lcd.setNumber(SolabasicHT1621::DISPLAY_INPUT, 123);

  // Display 2: OUTPUT / LOAD.
  lcd.setNumber(SolabasicHT1621::DISPLAY_OUTPUT, 456);
  delay(1500);

  // Ceros a la izquierda.
  lcd.setNumber(SolabasicHT1621::DISPLAY_INPUT, 7, true);   // 007
  lcd.setNumber(SolabasicHT1621::DISPLAY_OUTPUT, 8, true); // 008
  delay(1500);

  // Control individual de cada dígito.
  for (uint8_t display = 1; display <= 2; display++) {
    for (uint8_t digito = 1; digito <= 3; digito++) {
      for (uint8_t numero = 0; numero <= 9; numero++) {
        lcd.setDigit(display, digito, numero);
        delay(100);
      }
      lcd.clearDigit(display, digito);
    }
  }
}

void probarSegmentosAG() {
  lcd.clear();

  // Prueba individual de los 42 segmentos:
  // 2 displays x 3 dígitos x 7 segmentos.
  for (uint8_t display = 1; display <= 2; display++) {
    for (uint8_t digito = 1; digito <= 3; digito++) {
      for (char segmento = 'a'; segmento <= 'g'; segmento++) {
        lcd.setSegment(display, digito, segmento, true);
        delay(140);
        lcd.setSegment(display, digito, segmento, false);
      }
    }
  }
}

void probarIconos() {
  lcd.clear();

  // Enciende cada icono por separado.
  for (uint8_t indice = 0; indice < CANTIDAD_ICONOS; indice++) {
    lcd.setIcon(ICONOS[indice], true);
    delay(250);
    lcd.setIcon(ICONOS[indice], false);
  }

  // Enciende todos los iconos identificados al mismo tiempo.
  for (uint8_t indice = 0; indice < CANTIDAD_ICONOS; indice++) {
    lcd.setIcon(ICONOS[indice], true);
  }
  delay(1500);

  for (uint8_t indice = 0; indice < CANTIDAD_ICONOS; indice++) {
    lcd.setIcon(ICONOS[indice], false);
  }
}

void probarUnidadesYSimbolos() {
  lcd.clear();
  lcd.setNumber(SolabasicHT1621::DISPLAY_INPUT, 127);
  lcd.setNumber(SolabasicHT1621::DISPLAY_OUTPUT, 220);

  // Display 1: INPUT/BATTERY.
  lcd.setUnitVac(SolabasicHT1621::DISPLAY_INPUT, true);
  delay(500);
  lcd.setUnitVac(SolabasicHT1621::DISPLAY_INPUT, false);

  lcd.setUnitHz(SolabasicHT1621::DISPLAY_INPUT, true);
  delay(500);
  lcd.setUnitHz(SolabasicHT1621::DISPLAY_INPUT, false);

  lcd.setUnitVdc(SolabasicHT1621::DISPLAY_INPUT, true); // Vcc / DC
  delay(500);
  lcd.setUnitVdc(SolabasicHT1621::DISPLAY_INPUT, false);

  // Display 2: OUTPUT/LOAD.
  lcd.setUnitVac(SolabasicHT1621::DISPLAY_OUTPUT, true);
  delay(500);
  lcd.setUnitVac(SolabasicHT1621::DISPLAY_OUTPUT, false);

  lcd.setUnitHz(SolabasicHT1621::DISPLAY_OUTPUT, true);
  delay(500);
  lcd.setUnitHz(SolabasicHT1621::DISPLAY_OUTPUT, false);

  lcd.setPercent(true);
  delay(500);
  lcd.setPercent(false);

  // Elementos generales.
  lcd.setLowerFrame(true);
  delay(500);
  lcd.setLowerFrame(false);

  lcd.setAcIcon(true); // Onda de corriente alterna.
  delay(500);
  lcd.setAcIcon(false);

  lcd.setDcIcon(true); // Comparte E031 con Vcc/DC.
  delay(500);
  lcd.setDcIcon(false);
}

void probarBarras() {
  lcd.clear();

  for (uint8_t nivel = 0; nivel <= 4; nivel++) {
    lcd.setBarLevel(1, nivel);
    delay(300);
  }

  for (uint8_t nivel = 0; nivel <= 4; nivel++) {
    lcd.setBarLevel(2, nivel);
    delay(300);
  }

  // Control de un segmento específico de cada barra.
  lcd.setBarSegment(1, 0, false);
  lcd.toggleBarSegment(2, 3);
  delay(1000);
}

void recorrerLos128Elementos() {
  lcd.clear();

  // Acceso directo a todos los elementos físicos E000..E127.
  // Solamente queda uno encendido en cada paso.
  for (uint8_t elemento = 0; elemento < 128; elemento++) {
    lcd.setElement(elemento, true);
    delay(90);
    lcd.setElement(elemento, false);
  }
}

void mostrarPantallaFinal() {
  lcd.clear();

  lcd.setNumber(SolabasicHT1621::DISPLAY_INPUT, 127);
  lcd.setNumber(SolabasicHT1621::DISPLAY_OUTPUT, 220);

  lcd.setIcon(SolabasicHT1621::ICON_INPUT, true);
  lcd.setIcon(SolabasicHT1621::ICON_BATTERY, true);
  lcd.setIcon(SolabasicHT1621::ICON_OUTPUT, true);
  lcd.setIcon(SolabasicHT1621::ICON_LOAD, true);
  lcd.setIcon(SolabasicHT1621::ICON_LINE_MODE, true);
  lcd.setUnitVdc(SolabasicHT1621::DISPLAY_INPUT, true);
  lcd.setUnitVac(SolabasicHT1621::DISPLAY_OUTPUT, true);
  lcd.setLowerFrame(true);
  lcd.setAcIcon(true);

  lcd.setBarLevel(1, 3);
  lcd.setBarLevel(2, 4);
}

void setup() {
  lcd.begin();

  probarDisplaysNumericos();
  probarSegmentosAG();
  probarIconos();
  probarUnidadesYSimbolos();
  probarBarras();
  recorrerLos128Elementos();
  mostrarPantallaFinal();
}

void loop() {
  // La prueba se ejecuta una sola vez en setup().
  // Aquí puede colocar la lógica real de su aplicación.
}
