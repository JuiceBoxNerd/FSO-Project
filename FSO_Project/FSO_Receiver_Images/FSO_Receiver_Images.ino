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

const int receiver = A1;          // Analog pin to read light
const int recSpeed = 10000;       // Microseconds per bit
const int THRESHOLD = 100;        // Light level threshold
const int chunkSize = 8;          // Bits per byte
const int END_GAP = 30000;        // Time to consider transmission ended

volatile bool startReceiving = false;
volatile bool allowBitRead = false;

unsigned long cycle = 0;

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);  // Listen for start/sync signals
  Serial.begin(9600);
  pinMode(receiver, INPUT);
  Serial.println("📡 Receiver Ready");
}

void loop() {
  if (!startReceiving) return;

  if (!startSignal()) return;

  Serial.println("🚀 Receiving...");

  String binaryInput = getInput();
  allowBitRead = false;  // Stop reading after transmission is done

  Serial.println("📥 Binary Input:");
  Serial.println(binaryInput);

  Serial.println("🧠 Converting to bytes:");
  for (int i = 0; i < binaryInput.length(); i += chunkSize) {
    String byteStr = binaryInput.substring(i, min(i + chunkSize, binaryInput.length()));
    if (byteStr.length() == 8) {
      byte byteVal = strtoul(byteStr.c_str(), NULL, 2);
      Serial.print("Byte received: ");
      Serial.println(byteVal);
    } else {
      Serial.print("⚠️ Incomplete byte: ");
      Serial.println(byteStr);
    }
  }

  startReceiving = false;  // Wait for next 'S' signal
  Serial.println("✅ Done. Waiting for new start signal.");
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    if (c == 'S') {
      startReceiving = true;
    } else if (c == 'R') {
      cycle = micros();  // Resync timing
    }
  }
}

boolean startSignal() {
  if (!startReceiving) return false;

  delayMicroseconds(recSpeed / 6);
  delay(25);  // Give time for signal to stabilize
  cycle = micros();
  allowBitRead = true;  // ✅ Now allow reading bits
  return true;
}

String getInput() {
  String input = "";
  unsigned long lastBitTime = micros();

  while (true) {
    input = getBit(input);
    lastBitTime = micros();

    if (input.endsWith("11111111111100000000000011111111")) {
      input.remove(input.length() - 32);  // Remove END_MARKER
      break;
    }

    // Timeout check (failsafe)
    if (micros() - lastBitTime > END_GAP) break;
  }

  return input;
}

String getBit(String input) {
  if (!allowBitRead) return input;  // ✅ Don't read before start signal

  while (micros() - cycle < recSpeed);
  cycle += recSpeed;

  int val = analogRead(receiver);
  bool bit = (val > THRESHOLD);
  input += bit ? '1' : '0';

  Serial.print(bit ? '1' : '0');  // Optional real-time output
  return input;
}