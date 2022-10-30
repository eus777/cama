#include <string.h>
#include "BluetoothSerial.h"
#include "basicbutton.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define DEVICE_NAME "Cama 1"
#define LED_DELAY 1000
#define BT_EOL '\n'

// Buttons
#define TORSO_UP "TU"
#define TORSO_DOWN "TD"
#define TORSO_MASSAGE "TM"
#define LEGS_UP "LU"
#define LEGS_DOWN "LD"
#define LEGS_MASSAGE "LM"

// Global vars
BluetoothSerial SerialBT;
long lastTick = 0;
long ledAccum = 0;
bool ledStatus = false;

ButtonHandler buttonHandler;

void setup() {
  Serial.begin(9600);
  delay(250); // Sin este delay no imprime el log de SETUP
  Serial.println();
  Serial.print("SETUP: ");
  Serial.print(millis()); // micros()
  Serial.println(" ------------------------------");
  pinMode(LED_BUILTIN, OUTPUT); // ESP32S LED_BUILTIN = 2
  SerialBT.begin(DEVICE_NAME); //Bluetooth device name
  buttonHandler.add(new Button(TORSO_UP,      A15, PUSH));
  buttonHandler.add(new Button(TORSO_DOWN,    A16, PUSH));
  buttonHandler.add(new Button(TORSO_MASSAGE, A17, TOGGLE));
  buttonHandler.add(new Button(LEGS_UP,       A19, PUSH));
  buttonHandler.add(new Button(LEGS_DOWN,     A18, PUSH));
  buttonHandler.add(new Button(LEGS_MASSAGE,   A5, TOGGLE));
}

void loop() {
  ledAccum += tick();
  if (ledAccum >= LED_DELAY) {
    ledAccum -= LED_DELAY;
    ledStatus = !ledStatus;
    if (ledStatus) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
  }

  char btInput[STRING_SIZE] = "\0";
  readBT(btInput);
  buttonHandler.update(btInput);

  delay(5);
}

long tick() {
  long currentTick = millis();
  long delta = currentTick - lastTick;
  lastTick = currentTick;
  return delta;
}

/**
 * Copia una linea de SerialBT, copia un string vacío si no hay nada.
 */
void readBT(char* outStr) {

  char inBuf[READ_BUFFER_SIZE] = "\0";

  if (SerialBT.available()) {
    int eolIndex = SerialBT.readBytesUntil(BT_EOL, inBuf, READ_BUFFER_SIZE - 1);
    inBuf[eolIndex] = '\0';
    //Serial.println(inBuf);
    //Serial.print("Available: ");
    //Serial.println(SerialBT.available());
  }

  strcpy(outStr, inBuf);
}