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

#define RECEIVER 4
#define photodetectorPin A0
#define THRESHOLD 512
#define IMG_WIDTH 16
#define IMG_HEIGHT 16

unsigned long lastSample = 0;
const int bitInterval = 10000; // microseconds
bool receiving = false;
String currentChunk = "";
String fullMessage = "";

void setup() {
  Wire.begin(RECEIVER);
  Wire.onReceive(receiveEvent);

  pinMode(photodetectorPin, INPUT);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Ready to receive bits...");
}

void loop() {
  if (receiving && micros() - lastSample >= bitInterval) {
    lastSample = micros();
    char bit = getBit();
    currentChunk += bit;

    Serial.print(bit); // Print as received

    // End-of-chunk character
    if (currentChunk.endsWith(">")) {
      currentChunk.remove(currentChunk.length() - 1); // Remove '>'
      fullMessage += currentChunk;
      currentChunk = "";
      Serial.print(" [CHUNK DONE]\n");
    }

    // End of transmission
    if (fullMessage.endsWith("~*")) {
      Serial.println("\n\n[✓] Full message received.");
      fullMessage.remove(fullMessage.length() - 2); // Remove "~*"
      printReceivedImage(fullMessage);
      fullMessage = "";
    }
  }
}

char getBit() {
  int reading = analogRead(photodetectorPin);
  return (reading > THRESHOLD) ? '1' : '0';
}

void receiveEvent(int howMany) {
  char signal = Wire.read();
  if (signal == 'S') {
    receiving = true;
    currentChunk = "";
    lastSample = micros();
    Serial.println("\n[START SIGNAL RECEIVED]");
  } else if (signal == 'R') {
    Serial.print(" [SYNC] ");
  }
}

void printReceivedImage(String fullBinary) {
  Serial.println("\n\n📷 Image RGB Matrix:\n");

  int pixelCount = IMG_WIDTH * IMG_HEIGHT;
  int expectedBits = pixelCount * 24;

  if (fullBinary.length() < expectedBits) {
    Serial.print("❌ Error: Expected ");
    Serial.print(expectedBits);
    Serial.print(" bits, but got ");
    Serial.print(fullBinary.length());
    Serial.println(".");
    return;
  }

  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      int pixelStart = (y * IMG_WIDTH + x) * 24;

      byte r = strtol(fullBinary.substring(pixelStart, pixelStart + 8).c_str(), NULL, 2);
      byte g = strtol(fullBinary.substring(pixelStart + 8, pixelStart + 16).c_str(), NULL, 2);
      byte b = strtol(fullBinary.substring(pixelStart + 16, pixelStart + 24).c_str(), NULL, 2);

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

  Serial.println("\n✅ Transmission Done!\n");
}