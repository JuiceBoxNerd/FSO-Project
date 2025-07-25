#include <Wire.h>

const unsigned long bitDuration = 5000; // in microseconds (e.g. 5000 = 200 bits/sec)
const int transmitter = 2;
unsigned long cycle = 0;

void setup() {
  pinMode(transmitter, OUTPUT);
  digitalWrite(transmitter, LOW);

  Wire.begin(); // Master
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Enter a message to send:");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message += ">"; // Add end-of-message marker

    // Send start command to slave
    Wire.beginTransmission(8);
    Wire.write('S');
    Wire.endTransmission();
    delay(10); // Give receiver time to prepare

    cycle = micros(); // sync timing

    Serial.print("Sending: ");
    Serial.println(message);

    for (int i = 0; i < message.length(); i++) {
      sendByte(message[i]);
    }
  }
}

void sendByte(char c) {
  for (int i = 7; i >= 0; i--) {
    int bitVal = (c >> i) & 1;
    sendBit(bitVal);
    Serial.print(bitVal);
  }
  Serial.print(" ");
}

void sendBit(int bitVal) {
  digitalWrite(transmitter, bitVal ? HIGH : LOW);
  while (micros() - cycle < bitDuration); // wait for duration
  cycle += bitDuration; // next cycle start
}
