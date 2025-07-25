<<<<<<< HEAD
<<<<<<< HEAD
#include <Wire.h>

const unsigned long bitDuration = 5000; // in microseconds (e.g. 5000 = 200 bits/sec)
const int transmitter = 2;
unsigned long cycle = 0;
=======

const int transmitter = 2;
const unsigned long bitDurationMicros = 50000;  // 50 ms per bit
unsigned long cycleMicros = 0;
>>>>>>> parent of 932450e (Revert)
=======

const int transmitter = 2;
const unsigned long bitDurationMicros = 50000;  // 50 ms per bit
unsigned long cycleMicros = 0;
>>>>>>> parent of 932450e (Revert)

void setup() {
  pinMode(transmitter, OUTPUT);
  digitalWrite(transmitter, LOW);
<<<<<<< HEAD
<<<<<<< HEAD

  Wire.begin(); // Master
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Enter a message to send:");
=======
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
>>>>>>> parent of 932450e (Revert)
=======
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
>>>>>>> parent of 932450e (Revert)
}

void loop() {
  if (Serial.available()) {
<<<<<<< HEAD
<<<<<<< HEAD
    String message = Serial.readStringUntil('\n');
    message += ">"; // Add end-of-message marker

    // Send start command to slave
    Wire.beginTransmission(8);
    Wire.write('S');
    Wire.endTransmission();
    delay(10); // Give receiver time to prepare

    cycle = micros(); // sync timing

    Serial.print("Sending: ");
    Serial.println(message);

    for (int i = 0; i < message.length(); i++) {
      sendByte(message[i]);
    }
  }
}

void sendByte(char c) {
  for (int i = 7; i >= 0; i--) {
    int bitVal = (c >> i) & 1;
    sendBit(bitVal);
    Serial.print(bitVal);
  }
<<<<<<< Updated upstream
  Serial.print(" ");
=======
  else{
=======
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
>>>>>>> parent of 932450e (Revert)
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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
  cycle = cycle + sendSpeed;
<<<<<<< HEAD
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
}

void sendBit(int bitVal) {
  digitalWrite(transmitter, bitVal ? HIGH : LOW);
  while (micros() - cycle < bitDuration); // wait for duration
  cycle += bitDuration; // next cycle start
}
=======
>>>>>>> Stashed changes
=======
  cycle = cycle + sendSpeed;
>>>>>>> Stashed changes
}

=======
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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
  cycle = cycle + sendSpeed;
>>>>>>> Stashed changes
=======
  cycle = cycle + sendSpeed;
>>>>>>> Stashed changes
}

>>>>>>> parent of 932450e (Revert)
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


<<<<<<< HEAD
>>>>>>> parent of 932450e (Revert)
=======
>>>>>>> parent of 932450e (Revert)
