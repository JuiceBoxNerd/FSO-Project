#include <Wire.h>

const int sendSpeed = 25000;  // 50 ms in micros
const int transmitter = 2;

unsigned long cycle = micros();
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 128;  // Resync every 128 bits

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
  Wire.endTransmission();
  delayMicroseconds(100);  // Short buffer to allow handling
  Serial.print("[SYNC]");
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);  // microsecond-based delay
  cycle += sendSpeed;
  Serial.print(x);
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500);  // Keep in milliseconds for compatibility with Wire/I2C
  Wire.endTransmission();
  delay(500);  // Keep in milliseconds for compatibility
  cycle = micros();  // Reset the cycle timer using micros
}
