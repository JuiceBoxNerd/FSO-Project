#include <Wire.h>

const int sendSpeed = 50000; // 50 ms in micros
const int transmitter = 2;
unsigned long cycle = 0;

int bitsSent = 0;

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
    Serial.print("Sending: " + inputText + "\nBinary Output:\n");

    inputText += ">"; // Terminator

    startSignal();

    for (int i = 0; i < inputText.length(); i++) {
      char c = inputText[i];
      sendBinary(c);
      Serial.print(" ");
    }

    Serial.println("\n\nEnter another message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    sendBit((c >> i) & 1);
    Serial.print((c >> i) & 1);
  }
}

void sendBit(int bitVal) {
  digitalWrite(transmitter, bitVal ? HIGH : LOW);
  unsigned long start = micros();
  while (micros() - start < sendSpeed);
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  Wire.endTransmission();
  delay(10); // slight buffer before sending bits
}
