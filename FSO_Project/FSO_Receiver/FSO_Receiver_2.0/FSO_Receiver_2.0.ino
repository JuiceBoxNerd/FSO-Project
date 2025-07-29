#include <Wire.h>

volatile bool startReceiving = false;
const unsigned long recSpeed = 50000;  // 50 ms in micros
const int receiver = A3;
int threshold = 100;
String binaryInput = "";
String text = "";
int spaceCount = 0;
unsigned long nextBitTime = 0;

const String SYNC_PATTERN = "11111110";

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
    Serial.println("Decoded word is: " + text);
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
  while (true) {
    binaryInput = getBit(binaryInput);

    if (binaryInput.length() >= 8) {
      String byteCandidate = binaryInput.substring(0, 8);

      if (byteCandidate == SYNC_PATTERN) {
        Serial.println("\n*** Resync performed ***\n");
        binaryInput = "";
        nextBitTime = micros() + recSpeed;
        continue;
      }

      if (byteCandidate == "00111110") break;  // '>' terminator

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
      delay(60);
      startReceiving = true;
    }
  }
}

boolean startSignal() {
  if (!startReceiving) return false;
  delay(60);
  nextBitTime = micros() + recSpeed;
  return true;
}

String getBit(String input) {
  int samples = 10;
  int lightDetected = 0;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetected++;
    }
    delayMicroseconds((recSpeed / 12) / samples);
  }

  while (micros() < nextBitTime);
  nextBitTime += recSpeed;

  bool bit = lightDetected > (samples / 2);
  String newBit = bit ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  spaceCount++;
  if (spaceCount % 8 == 0) Serial.print(" ");
  if (spaceCount % 160 == 0) Serial.println();

  return output;
}
