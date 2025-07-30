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
    int bitValue = (bitChar == '1') ? HIGH : LOW;
    sendBit(bitValue, bitChar);  // Pass both value and character for printing
    bitsSentSinceResync++;
  }

  Serial.println();  // Final newline after all bits
  digitalWrite(transmitter, LOW); // Turn off laser after send
}

void sendBit(int bitValue, char bitChar) {
  digitalWrite(transmitter, bitValue);
  Serial.print(bitChar);  // Only print the bit when it's being sent
  Serial.print(" ");      // Space for readability

  while (micros() - cycle < sendSpeed);
  cycle += sendSpeed;
}

void sendResyncSignal() {
  Wire.beginTransmission(4);  // Receiver address
  Wire.write('R');
  Wire.endTransmission();
  cycle = micros();  // Reset cycle to allow receiver to catch up
  Serial.print(" [SYNC] ");
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(25);
  Wire.endTransmission();
  delay(25);
  cycle = micros();
  Serial.println("🚨 Start Signal Sent!");
}