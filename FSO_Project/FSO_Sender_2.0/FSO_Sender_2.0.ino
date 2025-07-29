#include <Wire.h>

const int sendSpeed = 10000; // microseconds
const int transmitter = 2;
const int RESYNC_INTERVAL = 64;
const int chunkSize = 8;

unsigned long cycle = micros();
int bitsSentSinceResync = 0;

void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
  bitsSentSinceResync = 0;
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting: " + inputText + "\nBinary Output:\n");

    // Add "~*" as terminator
    inputText += "~*";

    for (int start = 0; start < inputText.length(); start += chunkSize) {
      String chunk = inputText.substring(start, start + chunkSize);
      chunk += ">";  // Add '>' as end-of-chunk

      startSignal();
      for (int i = 0; i < chunk.length(); i++) {
        sendBinary(chunk[i]);
        Serial.print(" ");
      }
      Serial.println();
    }

    Serial.println("\nEnter another text message:");
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

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);
  cycle = micros();
  Serial.print(x);
}

void sendResyncSignal() {
  Wire.beginTransmission(4);
  Wire.write('R');
  cycle = micros();
  Wire.endTransmission();
  Serial.print("[SYNC]");
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500);
  Wire.endTransmission();
  delay(500);
  cycle = micros();
}
