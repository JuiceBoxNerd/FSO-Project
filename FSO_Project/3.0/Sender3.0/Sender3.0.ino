const unsigned int sendSpeed = 25000;     // microseconds per bit
const unsigned int startBuffer = 500;     // milliseconds
const int transmitter = 2;

unsigned long cycle = 0;
#define MAX_MSG_LEN 125                  // Safe limit for input
const int RESYNC_INTERVAL = 25;
const char RESYNC_BYTE = 0xFF;            

char inputText[MAX_MSG_LEN + 3];         // +2 for "~*" + null terminator

void setup() {
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    size_t len = Serial.readBytesUntil('\n', inputText, MAX_MSG_LEN);
    inputText[len] = '\0';

    // Append terminator "~*"
    if (len <= MAX_MSG_LEN - 2) {
      inputText[len++] = '~';
      inputText[len++] = '*';
    }
    inputText[len] = '\0';

    Serial.print("Transmitting: ");
    Serial.println(inputText);
    Serial.print("Binary Output: ");

    startSignal();

    int charCount = 0;
    for (size_t i = 0; i < len; i++) {
      sendBinary(inputText[i]);
      Serial.print(" ");
      charCount++;

      if (charCount >= RESYNC_INTERVAL) {
        // Send resync sequence (two '~' bytes)
        sendBinary(RESYNC_BYTE);
        sendBinary(RESYNC_BYTE);
        Serial.print(" ");
        Serial.print(" ");
        charCount = 0;
      }
    }

    Serial.println("\n\nEnter another text message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    sendBit((c >> i) & 1);
  }
}

void sendBit(bool bit) {
  digitalWrite(transmitter, bit ? HIGH : LOW);
  while (micros() - cycle < sendSpeed) yield();
  cycle = micros();
  Serial.print(bit);
}

void startSignal() {
  digitalWrite(transmitter, HIGH);
  delay(startBuffer);
  digitalWrite(transmitter, LOW);
  delay(startBuffer);
  cycle = micros();
}
