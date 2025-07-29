#include <Wire.h>

const int receiver = A3;
const int recSpeed = 5000; // 50 ms in micros
int threshold = 100;
volatile bool startReceiving = false;

String bitBuffer = "";
String text = "";
int spaceCount = 0;
unsigned long cycle = 0;

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);
  delay(500);
  threshold = initializeThreshold();
  Serial.println("Ready to receive.");
}

void loop() {
  if (startReceiving) {
    Serial.println("Receiving...");
    text = "";
    bitBuffer = "";

    int zeroByteCount = 0;

    while (true) {
      bitBuffer += readBit();

      if (bitBuffer.length() >= 8) {
        String byteStr = bitBuffer.substring(0, 8);
        bitBuffer = bitBuffer.substring(8);

        Serial.print(byteStr + " ");

        if (byteStr == "00111110") break; // Terminator
        if (byteStr == "00000000") {
          zeroByteCount++;
          if (zeroByteCount >= 10) break;
        } else {
          zeroByteCount = 0;
        }

        char c = (char)strtol(byteStr.c_str(), NULL, 2);
        text += c;

        if (++spaceCount % 8 == 0) Serial.println();
      }
    }

    Serial.println("\nMessage received: " + text + "\n");
    startReceiving = false;
  }
}

String readBit() {
  int lightDetected = 0;
  const int samples = 5;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) lightDetected++;
    delayMicroseconds((recSpeed / 3) / samples);
  }

  while (micros() - cycle < recSpeed);  // wait for full bit time
  cycle += recSpeed;

  return (lightDetected > (samples / 2)) ? "1" : "0";
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S') {
      delayMicroseconds(recSpeed / 3);  // sync midpoint
      cycle = micros();
      startReceiving = true;
    }
  }
}

int initializeThreshold() {
  long total = 0;
  int count = 0;
  unsigned long start = millis();

  while (millis() - start < 1000) {
    total += analogRead(receiver);
    count++;
  }

  int avg = total / count;
  int thresh = avg - 400;
  Serial.print("Threshold set to: ");
  Serial.println(thresh);
  return thresh;
}
