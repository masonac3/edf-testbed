/*
 * EDF Thrust Stand Data Logger
 *
 * Reads a 10 kg straight-bar load cell via HX711 and bus voltage/current
 * via INA226, and streams timestamped CSV over serial.
 *
 * SIMULATE mode lets the entire program run with no hardware attached, so
 * the serial protocol, calibration math, averaging and CSV formatting can
 * all be debugged before the parts arrive. Every hardware access is behind
 * a small number of functions; nothing else in the program touches a pin.
 *
 * Serial commands (send a single character, newline terminated):
 *   t   tare - capture current raw reading as zero
 *   c   calibration mode - prints raw counts continuously, no conversion
 *   r   run - begin CSV logging
 *   s   stop
 *   ?   print status
 *
 * Libraries (Library Manager):
 *   HX711 by Bogdan Necula
 *   INA226 by Rob Tillaart
 */

#define SIMULATE 1          // 1 = no hardware needed, 0 = real sensors

#if !SIMULATE
  #include "HX711.h"
  #include "INA226.h"
  #include <Wire.h>
  const uint8_t HX_DOUT = 2;
  const uint8_t HX_SCK  = 3;
  HX711 scale;
  INA226 ina(0x40);
#endif

// ---------------------------------------------------------------- config
const uint8_t  N_AVG        = 8;       // raw samples averaged per output row
const uint16_t OUT_PERIOD_MS = 100;    // 10 Hz output

// Calibration: grams = (raw - zeroOffset) / countsPerGram
// Fill countsPerGram in after running analysis/calibrate.py on a cal sweep.
float countsPerGram = 108.0;           // PLACEHOLDER - must be calibrated
long  zeroOffset    = 0;

// ---------------------------------------------------------------- state
enum Mode { IDLE, CALIBRATE, RUN };
Mode mode = IDLE;
unsigned long lastOut = 0;

// ------------------------------------------------- hardware access layer
// These four functions are the ONLY place hardware is touched.

long readLoadCellRaw() {
#if SIMULATE
  // Wander around the 24-bit midpoint with plausible noise so the
  // downstream code sees something realistic.
  static long fake = 8388608L;
  fake += random(-40, 41);
  return fake;
#else
  return scale.read();
#endif
}

float readBusVoltage() {
#if SIMULATE
  return 24.6 + (random(-30, 31) / 1000.0);
#else
  return ina.getBusVoltage();
#endif
}

float readCurrent() {
#if SIMULATE
  return 42.0 + (random(-200, 201) / 100.0);
#else
  return ina.getCurrent();
#endif
}

void initHardware() {
#if !SIMULATE
  scale.begin(HX_DOUT, HX_SCK);
  scale.set_gain(128);
  Wire.begin();
  ina.begin();
  ina.setMaxCurrentShunt(150, 0.00050);   // 150 A max, 0.5 mOhm shunt
#endif
}

// ---------------------------------------------------------------- helpers

long readRawAveraged(uint8_t n) {
  long long sum = 0;
  for (uint8_t i = 0; i < n; i++) sum += readLoadCellRaw();
  return (long)(sum / n);
}

float rawToGrams(long raw) {
  return (float)(raw - zeroOffset) / countsPerGram;
}

void doTare() {
  Serial.println("# taring, hold still...");
  zeroOffset = readRawAveraged(32);
  Serial.print("# zeroOffset = ");
  Serial.println(zeroOffset);
}

void printHeader() {
  Serial.println("time_ms,thrust_g,voltage_V,current_A,power_W,raw");
}

void printStatus() {
  Serial.println("# --- status ---");
  Serial.print("# simulate      : "); Serial.println(SIMULATE);
  Serial.print("# zeroOffset    : "); Serial.println(zeroOffset);
  Serial.print("# countsPerGram : "); Serial.println(countsPerGram, 4);
  Serial.print("# mode          : ");
  Serial.println(mode == IDLE ? "idle" : (mode == CALIBRATE ? "calibrate" : "run"));
}

// ---------------------------------------------------------------- setup

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  initHardware();
  randomSeed(analogRead(A0));

  Serial.println("# EDF thrust stand logger");
#if SIMULATE
  Serial.println("# *** SIMULATE MODE - readings are synthetic ***");
#endif
  Serial.println("# commands: t=tare  c=calibrate  r=run  s=stop  ?=status");
  doTare();
}

// ---------------------------------------------------------------- loop

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case 't': doTare(); break;
      case 'c':
        mode = CALIBRATE;
        Serial.println("# calibration mode: raw counts only");
        Serial.println("# hang known masses, record the steady value for each");
        Serial.println("time_ms,raw");
        break;
      case 'r':
        mode = RUN;
        printHeader();
        break;
      case 's':
        mode = IDLE;
        Serial.println("# stopped");
        break;
      case '?': printStatus(); break;
      default: break;
    }
  }

  if (mode == IDLE) return;
  if (millis() - lastOut < OUT_PERIOD_MS) return;
  lastOut = millis();

  long raw = readRawAveraged(N_AVG);

  if (mode == CALIBRATE) {
    Serial.print(millis()); Serial.print(',');
    Serial.println(raw);
    return;
  }

  float g = rawToGrams(raw);
  float v = readBusVoltage();
  float a = readCurrent();

  Serial.print(millis());   Serial.print(',');
  Serial.print(g, 1);       Serial.print(',');
  Serial.print(v, 3);       Serial.print(',');
  Serial.print(a, 2);       Serial.print(',');
  Serial.print(v * a, 1);   Serial.print(',');
  Serial.println(raw);
}
