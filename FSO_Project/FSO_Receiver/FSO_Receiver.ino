#include <Wire.h>

const int receiver = A3;
const unsigned long bitDuration = 5000; // match transmitter!
const int thresholdOffset = 400;
int threshold = 100;

String binaryInput = "";
String text = "";
bool isReceiving = false;
unsigned long cycle = 0;

void setup() {
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);

  Wire.begin(8); // Slave address 8
  Wire.onReceive(receiveCommand);

  threshold = calibrateThreshold();
  Serial.print("Threshold set to: ");
  Serial.println(threshold);
}

void loop() {
  if (isReceiving) {
    Serial.println("Receiving...");
    receiveMessage();
    Serial.println("Decoded: " + text);
    Serial.println();

    // Reset
    binaryInput = "";
    text = "";
    isReceiving = false;
  }
}

void receiveCommand(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S') {
      isReceiving = true;
      cycle = micros(); // Sync timing
    }
  }
}

int calibrateThreshold() {
  unsigned long total = 0;
  int samples = 500;

  for (int i = 0; i < samples; i++) {
    total += analogRead(receiver);
    delay(1);
  }
  return (total / samples) - thresholdOffset;
}

void receiveMessage() {
  while (true) {
    binaryInput += getBit();

    if (binaryInput.length() >= 8) {
      String byteStr = binaryInput.substring(0, 8);
      binaryInput = binaryInput.substring(8);

      if (byteStr == "00111110" || byteStr == "00000000") break; // '>' or null terminator

      char decoded = binaryToChar(byteStr);
      if (decoded >= 32 && decoded <= 126) {
        text += decoded;
      } else {
        Serial.print(" [Invalid: ");
        Serial.print(byteStr);
        Serial.print("] ");
      }
    }
  }
}

char binaryToChar(String byteStr) {
  return (char)strtol(byteStr.c_str(), NULL, 2);
}

String getBit() {
  const int samples = 10;
  int lightCount = 0;

  for (int i = 0; i < samples; i++) {
    int val = analogRead(receiver);
    if (val <= threshold) lightCount++;
    delayMicroseconds(bitDuration / samples);
  }

  while (micros() - cycle < bitDuration); // finish bit period
  cycle += bitDuration;

  return (lightCount > samples / 2) ? "1" : "0";
}
