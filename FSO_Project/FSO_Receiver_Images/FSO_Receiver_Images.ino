/*const int IMG_WIDTH = 20;
const int IMG_HEIGHT = 20;
const int IMG_SIZE = IMG_WIDTH * IMG_HEIGHT * 3; // 3 bytes per pixel (RGB) (Change to 1 for Grayscale)

String binaryInput = "";
byte image[IMG_SIZE];
bool ready = false;
int byteIndex = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Paste binary data for image (RGB888, 6144 bits for 16x16)");
  Serial.println("End input with >");
}

void loop() {
  if (!ready && Serial.available()) {
    char c = Serial.read();
    if (c == '>') {
      ready = true;
      Serial.println("\nFinished receiving data.");
      convertBinaryToImage();
      printImage();
    } else if (c == '0' || c == '1') {
      binaryInput += c;

      if (binaryInput.length() >= 8 && byteIndex < IMG_SIZE) {
        String byteStr = binaryInput.substring(0, 8);
        image[byteIndex++] = strtol(byteStr.c_str(), NULL, 2);
        binaryInput = binaryInput.substring(8);
      }
    }
  }
}

void convertBinaryToImage() {
  if (byteIndex < IMG_SIZE) {
    Serial.print("Warning: Only received ");
    Serial.print(byteIndex);
    Serial.println(" bytes. Filling rest with black.");
    for (int i = byteIndex; i < IMG_SIZE; i++) {
      image[i] = 0;
    }
  }
}

void printImage() {
  Serial.println("\nImage RGB matrix:");
  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      int index = (y * IMG_WIDTH + x) * 3;
        // ----------------------------------
      // FOR RGB
      byte r = image[index];
      byte g = image[index + 1];
      byte b = image[index + 2];
        // ----------------------------------
      // For Grayscale:
      // byte grayscale = image[index];
        // ----------------------------------
      // Print the RGB values for the current pixel
      Serial.print("(");
      Serial.print(r);
      Serial.print(",");
      Serial.print(g);
      Serial.print(",");
      Serial.print(b);
      Serial.print(")");
        // ----------------------------------
      // FOR GRAYSCALE: Print the Grayscale values for the current pixel
      // Serial.print(grayscale);
        // ----------------------------------
      // Print a comma after each pixel, except for the last one in each row
      if (x < IMG_WIDTH - 1) {
        Serial.print(", ");
      }
    }

    // Add a comma at the end of each row, except for the last row
    if (y < IMG_HEIGHT - 1) {
      Serial.print(", ");
    }

    // Move to the next line after printing one row of pixels
    Serial.println();
  }
}*/

/*
██╗██████╗░██╗░░░░░  ░█████╗░░█████╗░██████╗░███████╗
██║██╔══██╗██║░░░░░  ██╔══██╗██╔══██╗██╔══██╗██╔════╝
██║██████╔╝██║░░░░░  ██║░░╚═╝██║░░██║██║░░██║█████╗░░
██║██╔══██╗██║░░░░░  ██║░░██╗██║░░██║██║░░██║██╔══╝░░
██║██║░░██║███████╗  ╚█████╔╝╚█████╔╝██████╔╝███████╗
╚═╝╚═╝░░╚═╝╚══════╝  ░╚════╝░░╚════╝░╚═════╝░╚══════╝
*/


#include <Wire.h>

volatile bool startReceiving = false;
volatile bool resyncRequested = false;

const int recSpeed = 10000;   // microseconds per bit
const int receiver = 2;       // photodetector pin

#define IMG_WIDTH 4
#define IMG_HEIGHT 4

String binaryInput = "";
String currentChunk = "";
String fullMessage = "";

int spaceCount = 0;
int zeroCount = 0;
unsigned long cycle = micros();

const int ZERO_END_COUNT = 10;

void setup() {
  Wire.begin(4);  // I2C address
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

    Serial.println("Full binary message received.");
    printImage(fullMessage);
    fullMessage = ""; // reset
    startReceiving = false;
  }

  binaryInput = "";
  currentChunk = "";
  zeroCount = 0;
}

char binaryToChar(String byteStr) {
  return (char)strtol(byteStr.c_str(), NULL, 2);
}

void getInput() {
  while (true) {
    binaryInput = getBit(binaryInput);

    // Check for full byte
    if (binaryInput.length() >= 8) {
      String byteCandidate = binaryInput.substring(0, 8);
      char decodedChar = binaryToChar(byteCandidate);
      currentChunk += decodedChar;
      binaryInput = binaryInput.substring(8);
    }

    // End condition: 50 consecutive zeros
    if (zeroCount >= ZERO_END_COUNT) {
      Serial.println("\n🛑 50 consecutive 0s detected (end of transmission).");
      break;
    }
  }
  Serial.println();
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'S') {
      delayMicroseconds(recSpeed / 6);
      delay(25);
      startReceiving = true;
    } else if (cmd == 'R') {
      resyncRequested = true;
    }
  }
}

boolean startSignal() {
  if (!startReceiving) return false;
  delayMicroseconds(recSpeed / 6);
  delay(25);
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
      if (newBit == "0") zeroCount++;
      else zeroCount = 0;
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

  if (newBit == "0") zeroCount++;
  else zeroCount = 0;

  return input + newBit;
}

void printImage(String binaryStr) {
  Serial.println("\n\n📷 Image RGB Matrix:\n");

  const int totalPixels = IMG_WIDTH * IMG_HEIGHT;
  const int expectedBits = totalPixels * 24;

  if (binaryStr.length() < expectedBits) {
    Serial.print("❌ Error: Expected ");
    Serial.print(expectedBits);
    Serial.print(" bits, got ");
    Serial.println(binaryStr.length());
    return;
  }

  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      int pixelIndex = (y * IMG_WIDTH + x) * 24;

      byte r = strtol(binaryStr.substring(pixelIndex, pixelIndex + 8).c_str(), NULL, 2);
      byte g = strtol(binaryStr.substring(pixelIndex + 8, pixelIndex + 16).c_str(), NULL, 2);
      byte b = strtol(binaryStr.substring(pixelIndex + 16, pixelIndex + 24).c_str(), NULL, 2);

      Serial.print("(");
      Serial.print(r);
      Serial.print(",");
      Serial.print(g);
      Serial.print(",");
      Serial.print(b);
      Serial.print(")");

      if (x < IMG_WIDTH - 1) Serial.print(", ");
    }
    Serial.println();
  }

  Serial.println("\n✅ RGB image print complete.");
}