#include <Wire.h>
volatile bool startReceiving = false;
volatile bool resyncRequested = false;

const int recSpeed = 50000;  // 50ms in micros
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
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
  delay(500);
  threshold = initializer();
  Serial.println();
}

void loop() {
  if (startSignal()) {
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text);
    Serial.println();
    startReceiving = false;
  }
  binaryInput = "";
  text = "";
}

int initializer() {
  unsigned long start = millis();
  long total = 0;
  int count = 0;

  while (millis() - start < 1000) {
    int reading = analogRead(receiver);
    total += reading;
    count++;
  }

  int average = total / count;
  Serial.print("Final Threshold: ");
  Serial.println(average - 400);
  return average - 400;
}

char binaryToChar(String byteStr) {
  return (char)strtol(byteStr.c_str(), NULL, 2);
}

void getInput() {
  int stopCount = 0;
  while (true) {
    binaryInput = getBit(binaryInput);

    if (binaryInput.length() >= 8) {
      String byteCandidate = binaryInput.substring(0, 8);

      if (byteCandidate == "00111110") break; // '>' terminator
      if (byteCandidate == "00000000") {
        stopCount++;
        if (stopCount >= 20) break;
      } else {
        stopCount = 0;
      }

      char decodedChar = binaryToChar(byteCandidate);
      text += decodedChar;
      binaryInput = binaryInput.substring(8);
    }
  }
  Serial.println();
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S' || cmd == 's') {
      delayMicroseconds(recSpeed / 6);
      delay(500);
      startReceiving = true;
    } else if (cmd == 'R') {
      resyncRequested = true;
    }
  }
}

boolean startSignal() {
  if (!startReceiving) return false;
  delayMicroseconds(recSpeed / 6);
  delay(500);
  cycle = micros();
  return true;
}

String getBit(String input) {
  if (resyncRequested) {
    cycle = micros();  // Reset on resync
    delay(recSpeed/2)
    Serial.println("\n*** Resync performed (I2C) ***\n");
    resyncRequested = false;
  }

  int samples = 10;
  int lightDetected = 0;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetected++;
    }
    delayMicroseconds((recSpeed / 3) / samples);  // short delay between samples
  }

  while (micros() - cycle < recSpeed) {}
  cycle += recSpeed;

  bool bit = lightDetected > (samples / 2);
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
