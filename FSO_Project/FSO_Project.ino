
const int transmitter = 2;
const unsigned long bitDurationMicros = 50000;  // 50 ms per bit
unsigned long cycleMicros = 0;

void setup() {
  pinMode(transmitter, OUTPUT);
  digitalWrite(transmitter, LOW);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    inputText += '>';  // end marker
    Serial.print("Transmitting: ");
    Serial.println(inputText);
    startBit();
    cycleMicros = micros();

    for (int i = 0; i < inputText.length(); i++) {
      sendBinaryManchester(inputText[i]);
      Serial.print(" ");
    }
    Serial.println("\nDone transmitting.\nEnter another text message:");
  }
}

void sendManchesterBit(int bit) {
  unsigned long halfBitDuration = bitDurationMicros / 2;

  // For Manchester encoding:
  // 0 bit = LOW then HIGH
  // 1 bit = HIGH then LOW

  if (bit == 0) {
    digitalWrite(transmitter, LOW);
    waitUntil(cycleMicros + halfBitDuration);
    cycleMicros += halfBitDuration;

    digitalWrite(transmitter, HIGH);
    waitUntil(cycleMicros + halfBitDuration);
    cycleMicros += halfBitDuration;
  } else {
    digitalWrite(transmitter, HIGH);
    waitUntil(cycleMicros + halfBitDuration);
    cycleMicros += halfBitDuration;

    digitalWrite(transmitter, LOW);
    waitUntil(cycleMicros + halfBitDuration);
    cycleMicros += halfBitDuration;
  }
}

void sendBinaryManchester(char c) {
  for (int i = 7; i >= 0; i--) {
    int bit = (c >> i) & 1;
    sendManchesterBit(bit);
    Serial.print(bit);
  }
}

void waitUntil(unsigned long targetMicros) {
  while (micros() < targetMicros) {
    // busy wait
  }
}

void startBit() {
  // Send a HIGH signal for 500 ms (sync pulse)
  digitalWrite(transmitter, HIGH);
  unsigned long syncStart = micros();
  while (micros() - syncStart < 500000) {
    // wait for 500 ms
  }

  // Send a LOW signal for 500 ms (idle gap after sync)
  digitalWrite(transmitter, LOW);
  unsigned long gapStart = micros();
  while (micros() - gapStart < 500000) {
    // wait for another 500 ms
  }
}


