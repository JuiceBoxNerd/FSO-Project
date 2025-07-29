#include <Wire.h>

volatile bool startReceiving = false;
volatile bool resyncRequested = false;

const int recSpeed = 10000;
const int receiver = 2;

String binaryInput = "";
String currentChunk = "";
String fullMessage = "";

int spaceCount = 0;
unsigned long cycle = micros();

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Ready to receive...");
}

void loop() {
  if (startSignal()) {
    Serial.println("Receiving chunk...");
    getInput();

    Serial.println("Chunk received: " + currentChunk);
    fullMessage += currentChunk;

    if (currentChunk.endsWith("~*")) {
      Serial.println("Full message received:");
      fullMessage.remove(fullMessage.length() - 2); // remove "~*"
      Serial.println(fullMessage);
      fullMessage = ""; // reset for next message
    }

    startReceiving = false;
  }

  binaryInput = "";
  currentChunk = "";
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
      currentChunk += decodedChar;
      binaryInput = binaryInput.substring(8);
    }
  }
  Serial.println();
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S') {
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
    if (micros() - cycle > recSpeed / 2) {
      int samples = 10;
      int lightDetected = 0;
      for (int i = 0; i < samples; i++) {
        if (!digitalRead(receiver)) lightDetected++;
        delayMicroseconds((recSpeed / 6) / samples);
      }

      bool bit = lightDetected > (samples / 2);
      String newBit = bit ? "1" : "0";
      resyncRequested = false;
      Serial.print(newBit);
      spaceCount++;
      cycle = micros();
      Serial.println("\n*** Resync performed (I2C) ***\n");
      return input + newBit;
    }
  }

  int samples = 10;
  int lightDetected = 0;
  for (int i = 0; i < samples; i++) {
    if (!digitalRead(receiver)) lightDetected++;
    delayMicroseconds((recSpeed / 6) / samples);
  }

  while (micros() - cycle < recSpeed) {}
  cycle += recSpeed;

  bool bit = lightDetected > (samples / 2);
  String newBit = bit ? "1" : "0";
  Serial.print(newBit);
  spaceCount++;
  if (spaceCount % 8 == 0) Serial.print(" ");
  if (spaceCount >= 160) {
    Serial.println();
    spaceCount = 0;
  }

  return input + newBit;
}
