#include <Wire.h>
volatile bool startReceiving = false;
volatile bool resyncRequested = false;
const int recSpeed = 10;
const int receiver = 2;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;
long cycle = millis();

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
    int reading = digitalRead(receiver);
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
      delay(recSpeed / 3 + 500);
      startReceiving = true;
    } else if (cmd == 'R') {
      resyncRequested = true;
    }
  }
}

boolean startSignal() {
  if (!startReceiving) return false;
  delay(recSpeed / 2 + 500);
  cycle = millis();
  return true;
}

String getBit(String input) {
  if (resyncRequested) {
    if(millis()-cycle > sendSpeed/2){
     bool bit = lightDetected > (samples / 2);
     String newBit = bit ? "1" : "0";
     String output = input + newBit;
     Serial.print(newBit);
     spaceCount++;
    }
    cycle = millis();
    Serial.println("\n*** Resync performed (I2C) ***\n");
    resyncRequested = false;
  }

  int samples = 10;
  int lightDetected = 0;
  for (int i = 0; i < samples; i++) {
    if (!digitalRead(receiver)) {
      lightDetected++;
    }
    delay((recSpeed / 6) / samples);
  }
  while (millis() - cycle < recSpeed) {}
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
