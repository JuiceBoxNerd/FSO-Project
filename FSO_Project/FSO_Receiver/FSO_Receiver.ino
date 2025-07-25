#include <Wire.h>

const int recSpeed = 50;
const int receiver = A3;
int threshold = 100;
int spaceCount = 0;
long cycle = millis();
String binaryInput = "";
String text = "";
volatile bool isReceiving = false; // I2C trigger flag

void setup() {
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while(!Serial);
  delay(500);

  Wire.begin(8);  // I2C Slave address 8
  Wire.onReceive(receiveCommand);

  threshold = initializer();
  Serial.println();
}

void loop() {
  if (isReceiving) {
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text);
    Serial.println();
    text = "";
    binaryInput = "";
    isReceiving = false;
  }
}

void receiveCommand(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    if (c == 'S') {
      isReceiving = true;
    }
  }
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

    if (binaryInput.length() % 8 == 0) {
      String lastByte = binaryInput.substring(binaryInput.length() - 8);
      if (lastByte == "00111110" || lastByte == "00000000") {
        break;  // End of message
      }

      String currentByte = binaryInput.substring(0, 8);
      text += binaryToChar(currentByte);
      binaryInput = "";
    }
  }
  Serial.println();
}

String getBit(String input) {
  int samples = 10;
  int lightDetectedCount = 0;

  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetectedCount++;
    }
    delay((recSpeed / 12) / samples);
  }

  while (millis() - cycle < recSpeed) {}
  cycle = cycle  + recSpeed;

  bool detected = (lightDetectedCount > samples / 2);
  String newBit = detected ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
    spaceCount++;
  }

  if (spaceCount >= 20) {
    Serial.println();
    spaceCount = 0;
  }

  return output;
}