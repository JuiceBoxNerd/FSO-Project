#include <Wire.h>

const int sendSpeed = 50000;  // 50ms per bit (in microseconds)
const int transmitter = 2;
unsigned long cycle = micros();
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 128;

void setup() {
  Wire.begin();
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting: " + inputText + "\nBinary Output: ");
    inputText += ">";  // Terminator

    startSignal();

    for (int i = 0; i < inputText.length(); i++) {
      sendBinary(inputText[i]);
      Serial.print(" ");
    }

    Serial.println("\n\nEnter another text message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    if (bitsSentSinceResync >= RESYNC_INTERVAL) {
      sendResyncSignal();
      bitsSentSinceResync = 0;
    }
    sendBit((c >> i) & 1);
    bitsSentSinceResync++;
  }
}

void sendBit(int bitVal) {
  digitalWrite(transmitter, bitVal ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);
  cycle += sendSpeed;
  Serial.print(bitVal);
}

void sendResyncSignal() {
  Wire.beginTransmission(4);
  Wire.write('R');
  Wire.endTransmission();
  delayMicroseconds(200);
  Serial.print("[SYNC]");
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  Wire.endTransmission();
  delay(5);
  cycle = micros();
}
