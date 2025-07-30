#include <Wire.h>

const int sendSpeed = 10000;       // microseconds per bit
const int transmitter = 2;         // Laser or LED pin
const int RESYNC_INTERVAL = 64;    // Sync every 64 bits
const int chunkSize = 8;           // For spacing in Serial print

const String END_MARKER = "11111111111100000000000011111111";

unsigned long cycle = 0;
int bitsSentSinceResync = 0;

void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  digitalWrite(transmitter, LOW); // Laser OFF by default

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Enter a long binary string to transmit:");
  bitsSentSinceResync = 0;
}

void loop() {
  static String inputBuffer = "";

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer += END_MARKER;
      transmitLongBinary(inputBuffer);
      inputBuffer = "";
      Serial.println("\n✅ Transmission Done!\nEnter another binary string:");
    } else if (c == '0' || c == '1') {
      inputBuffer += c;
    }
  }
}

void transmitLongBinary(const String& binary) {
  Serial.print("Transmitting ");
  Serial.print(binary.length());
  Serial.println(" bits...");

  startSignal();
  delayMicroseconds(sendSpeed);  // Give receiver time to stabilize

  cycle = micros();  // Start timing here

  for (int i = 0; i < binary.length(); i++) {
    if (bitsSentSinceResync >= RESYNC_INTERVAL) {
      sendResyncSignal();
      bitsSentSinceResync = 0;
    }

    char bitChar = binary[i];
    int bitValue = (bitChar == '1') ? 1 : 0;
    sendBit(bitValue);
    bitsSentSinceResync++;

    // Debug spacing
    if ((i + 1) % chunkSize == 0) Serial.print(" ");
  }

  Serial.println();  // Final newline
  digitalWrite(transmitter, LOW); // Turn off laser after send
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);  // Wait full bit time
  cycle += sendSpeed;
  Serial.print(x);  // Debug
}

void sendResyncSignal() {
  Wire.beginTransmission(4);  // Receiver address
  Wire.write('R');
  Wire.endTransmission();
  cycle = micros();  // Reset cycle to allow receiver to catch up
  Serial.print(" [SYNC] ");
}

void startSignal() {
  Wire.beginTransmission(4);  // Receiver address
  Wire.write('S');
  Wire.endTransmission();
  delay(25);  // Give time for receiver to get ready
  Serial.println("🚨 Start Signal Sent!");
}