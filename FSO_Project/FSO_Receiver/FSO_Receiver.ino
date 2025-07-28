#include <Wire.h>
volatile bool startReceiving = false;
const int recSpeed = 50;
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;
long cycle = millis();

const int SYNC_WINDOW = 8;
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

      if (byteCandidate == SYNC_PATTERN) {
        Serial.println("\n*** Resync performed ***\n");
        binaryInput = "";  // Clear and wait for real data
        cycle = millis();
        continue;
      }

      if (byteCandidate == "00111110"){
        break;  // Terminator received
      }
      else if(byteCandidate == "00000000"){
        stopCount++;
      }
      if(stopCount >= 20){
        break;
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
    if (cmd == 's' || cmd == 'S') {
      delay(recSpeed / 2 + 500);
      startReceiving = true;
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
  int samples = 10;
  int lightDetected = 0;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetected++;
    }
    delay((recSpeed / 12) / samples);
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
