#include <Wire.h>

const int sendSpeed = 10000; // microseconds per bit
const int transmitter = 2;
unsigned long cycle = micros();

const int chunkSize = 8; // You can change this to split message differently

void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting: " + inputText + "\nBinary Output:\n");

    // Break input into chunks of `chunkSize`
    for (int start = 0; start < inputText.length(); start += chunkSize) {
      String chunk = inputText.substring(start, start + chunkSize);
      chunk += ">"; // Terminator

      startSignal(); // Treat each chunk as a new message

      for (int i = 0; i < chunk.length(); i++) {
        char c = chunk[i];
        sendBinary(c);
        Serial.print(" ");
      }

      Serial.println(); // Spacing between chunks
    }

    Serial.println("\nEnter another text message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    sendBit((c >> i) & 1);
  }
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (micros() - cycle < sendSpeed);
  cycle = micros();
  Serial.print(x);
}

void startSignal() {
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500); // Give the receiver time to get ready
  Wire.endTransmission();
  delay(500); // Allow setup before transmission
  cycle = micros(); // Reset timing cycle
}
