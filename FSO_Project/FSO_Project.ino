#include <Wire.h>
const int sendSpeed = 50;
const int transmitter = 2;

long cycle = millis();
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 64;  // Resync every 64 bits
const byte SYNC_BYTE = 0b11111110;

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
    Serial.print("Transmitting: " + inputText + "\nBinary Output: ");
    inputText += ">";  // Use '>' (00111110) as terminator
    startSignal();

    for (int i = 0; i < inputText.length(); i++) {
      char c = inputText[i];
      sendBinary(c);
      Serial.print(" ");
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

    sendBit((c >> i) & 1);
    bitsSentSinceResync++;
  }
}

void sendResyncByte() {
  Serial.print("[SYNC]");
  for (int i = 7; i >= 0; i--) {
    sendBit((SYNC_BYTE >> i) & 1);
  }
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (millis() - cycle < sendSpeed);
  cycle = millis();
  Serial.print(x);
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500);
  Wire.endTransmission();
  delay(500);
  cycle = millis();
}
