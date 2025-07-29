#include <Wire.h>
volatile bool startReceiving = false;
const int recSpeed = 10000; // microseconds-based speed
const int receiver = 2;
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
      binaryInput = binaryIn
