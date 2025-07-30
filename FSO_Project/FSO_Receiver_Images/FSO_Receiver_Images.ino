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

const int recSpeed = 25000;   // microseconds per bit
const int receiver = 2;       // photodetector pin

#define IMG_WIDTH 16
#define IMG_HEIGHT 16

String fullMessage = "";
unsigned long cycle = micros();

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
    Serial.println("Receiving image data...");
    fullMessage = getRawBits();

    Serial.println("\n📥 Raw binary received:");
    printBitSummary(fullMessage);

    if (fullMessage.length() >= IMG_WIDTH * IMG_HEIGHT * 24) {
      printImage(fullMessage);
    } else {
      Serial.println("❌ Not enough data for full image.");
    }

    fullMessage = "";
    startReceiving = false;
  }
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

String getRawBits() {
  String result = "";
  int totalBits = IMG_WIDTH * IMG_HEIGHT * 24;
  for (int i = 0; i < totalBits; i++) {
    result += readBit();
  }
  return result;
}

String readBit() {
  int samples = 10;
  int lightDetected = 0;
  for (int i = 0; i < samples; i++) {
    if (!digitalRead(receiver)) lightDetected++;
    delayMicroseconds((recSpeed / 6) / samples);
  }

  while (micros() - cycle < recSpeed) {}
  cycle += recSpeed;

  return (lightDetected > samples / 2) ? "1" : "0";
}

void printBitSummary(const String &bits) {
  for (int i = 0; i < bits.length(); i++) {
    Serial.print(bits[i]);
    if ((i + 1) % 8 == 0) Serial.print(" ");
    if ((i + 1) % 160 == 0) Serial.println();
  }
  Serial.println();
}

void printImage(String binaryStr) {
  Serial.println("\n🖼️ RGB Matrix:");

  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      int i = (y * IMG_WIDTH + x) * 24;
      byte r = strtol(binaryStr.substring(i, i + 8).c_str(), NULL, 2);
      byte g = strtol(binaryStr.substring(i + 8, i + 16).c_str(), NULL, 2);
      byte b = strtol(binaryStr.substring(i + 16, i + 24).c_str(), NULL, 2);

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

  Serial.println("\n✅ Image rendering done.");
}