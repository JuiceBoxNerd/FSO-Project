#include <Wire.h>
const int sendSpeed = 10000; // microseconds-based
const int transmitter = 2;

unsigned long cycle = micros();
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 64;  // Resync every 64 bits

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
    Serial.print("Transmitting: " + inputText + "\nBinary Output: ");
    inputText += ">";  // Use '>' as terminator
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
      sendResyncSignal();
      bitsSentSinceResync = 0;
    }

    sendBit((c >> i) & 1);
    bitsSentSinceResync++;
  }
}

void sendResyncSignal() {
  Wire.beginTransmission(4);
  Wire.write('R');  // Signal resync
  cycle = micros();
  Wire.endTransmission();
  Serial.print("[SYNC]");
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);
  cycle = micros();
  Serial.print(x);
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500); // still milliseconds, appropriate for I2C prep
  Wire.endTransmission();
  delay(500); // same
  cycle = micros();
}
