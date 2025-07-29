#include <Wire.h>

const unsigned long sendSpeed = 50000;  // 50 ms in micros
const int transmitter = 2;

unsigned long nextBitTime = 0;
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 128;
const byte SYNC_BYTE = 0b11111110;
int bitGroupCount = 0;

void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting: " + inputText + "\nBinary Output:\n");
    inputText += ">";  // Terminator
    startSignal();

    for (int i = 0; i < inputText.length(); i++) {
      char c = inputText[i];
      sendBinary(c);
    }

    Serial.println("\n\nEnter another text message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    if (bitsSentSinceResync >= RESYNC_INTERVAL) {
      sendResyncByte();
      bitsSentSinceResync = 0;
    }

    int bit = (c >> i) & 1;
    sendBit(bit);
    Serial.print(bit);

    bitsSentSinceResync++;

    if (++bitGroupCount % 8 == 0) Serial.print(" ");
    if (bitGroupCount % 160 == 0) Serial.println();
  }
}

void sendResyncByte() {
  Serial.print("[SYNC] ");
  for (int i = 7; i >= 0; i--) {
    int bit = (SYNC_BYTE >> i) & 1;
    sendBit(bit);
    Serial.print(bit);

    bitsSentSinceResync++;

    if (++bitGroupCount % 8 == 0) Serial.print(" ");
    if (bitGroupCount % 160 == 0) Serial.println();
  }
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() < nextBitTime);
  nextBitTime += sendSpeed;
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  Wire.endTransmission();
  delay(100);
  nextBitTime = micros() + sendSpeed;
}
