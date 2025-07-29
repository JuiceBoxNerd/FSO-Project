#include <Wire.h>

volatile bool startReceiving = false;
volatile bool resyncRequested = false;

const int recSpeed = 50000;  // 50ms per bit (in microseconds)
const int receiver = A3;
int threshold = 100;

String binaryInput = "";
String text = "";
int spaceCount = 0;
unsigned long cycle = micros();

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);
  delay(200);
  threshold = initializer();
}

void loop() {
  if (startSignal()) {
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text + "\n");
    startReceiving = false;
  }
  binaryInput = "";
  text = "";
}

int initializer() {
  unsigned long start = millis();
  long total = 0;
  int count = 0;

  while (millis() - start < 500) {
    total += analogRead(receiver);
    count++;
  }

  int avg = total / count;
  Serial.print("Threshold: ");
  Serial.println(avg - 400);
  return avg - 400;
}

char binaryToChar(String byteStr) {
  return (char)strtol(byteStr.c_str(), NULL, 2);
}

void getInput() {
  int stopCount = 0;

  while (true) {
    binaryInput = getBit(binaryInput);

    if (binaryInput.length() >= 8) {
      String byteStr = binaryInput.substring(0, 8);

      if (byteStr == "00111110") break;  // '>' terminator
      if (byteStr == "00000000") {
        stopCount++;
        if (stopCount >= 20) break;
      } else {
        stopCount = 0;
      }

      text += binaryToChar(byteStr);
      binaryInput = binaryInput.substring(8);
    }
  }
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S' || cmd == 's') {
      delayMicroseconds(recSpeed / 2);
      startReceiving = true;
    } else if (cmd == 'R') {
      resyncRequested = true;
    }
  }
}

bool startSignal() {
  if (!startReceiving) return false;
  delayMicroseconds(recSpeed / 2);
  cycle = micros();
  return true;
}

String getBit(String input) {
  if (resyncRequested) {
    cycle = micros();
    Serial.println("\n*** Resync Performed (I2C) ***\n");
    resyncRequested = false;
  }

  int samples = 10;
  int lightDetected = 0;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) lightDetected++;
    delayMicroseconds((recSpeed / 2) / samples);
  }

  while (micros() - cycle < recSpeed);
  cycle += recSpeed;

  bool bit = (lightDetected > samples / 2);
  String newBit = bit ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  spaceCount++;
  if (spaceCount % 8 == 0) Serial.print(" ");
  if (spaceCount >= 160) {
    Serial.println();
    spaceCount = 0;
  }

  return output;
}
