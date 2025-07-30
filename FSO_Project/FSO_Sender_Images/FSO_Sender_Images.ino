#include <Wire.h>

const int sendSpeed = 10000; // microseconds
const int transmitter = 2;
const int RESYNC_INTERVAL = 64;
const int chunkSize = 8; // You can adjust this for the chunk size

unsigned long cycle = micros();
int bitsSentSinceResync = 0;

void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a binary string:");
  bitsSentSinceResync = 0;
}

void loop() {
  if (Serial.available()) {
    String inputBinary = Serial.readStringUntil('\n');
    inputBinary.trim(); // Remove any leading/trailing whitespace

    // Ensure the binary string has a length that's a multiple of 8
    while (inputBinary.length() % 8 != 0) {
      inputBinary += "0"; // Pad with zeroes to make it a multiple of 8
    }

    Serial.print("Transmitting binary data: ");
    Serial.println(inputBinary);

    // Send the binary data in chunks of 8 bits
    for (int start = 0; start < inputBinary.length(); start += chunkSize) {
      String chunk = inputBinary.substring(start, start + chunkSize);
      startSignal();
      for (int i = 0; i < chunk.length(); i++) {
        sendBit(chunk[i] == '1' ? 1 : 0); // Send '1' or '0'
      }
      Serial.println();  // Newline after each chunk
    }

    Serial.println("Transmission Done!\nEnter another binary string:");
  }
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);
  cycle = micros();
  Serial.print(x); // Print the bit being sent
}

void sendResyncSignal() {
  Wire.beginTransmission(4);
  Wire.write('R');
  cycle = micros();
  Wire.endTransmission();
  Serial.print("[SYNC]");
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(25);
  Wire.endTransmission();
  delay(25);
  cycle = micros();
}