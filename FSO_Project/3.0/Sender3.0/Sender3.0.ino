<<<<<<< Updated upstream
const int sendSpeed = 10000;       // microseconds per bit
const int startBuffer = 100;       // milliseconds
=======
const int sendSpeed = 5000;
const int startBuffer = 500;
>>>>>>> Stashed changes
const int transmitter = 2;

const char RESYNC_BYTE = 0xFF;
const int RESYNC_INTERVAL = 25;

const int MAX_TEXT_LENGTH = 500;   // total safe message length
const int CHUNK_SIZE = 50;         // per transmission loop

char message[MAX_TEXT_LENGTH + 1];
char chunk[CHUNK_SIZE + 1];

long cycle = micros();

void setup() {
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message (max 500 chars):");
}

void loop() {
  if (Serial.available()) {
    int len = readSerialInput(message, MAX_TEXT_LENGTH);
    if (len <= 0) return;

    appendTerminator(message, len);
    Serial.print("Transmitting: ");
    Serial.println(message);

    startSignal();

    int totalChars = strlen(message);
    int charIndex = 0;
    int charsSinceResync = 0;

    while (charIndex < totalChars) {
      int chunkLen = min(CHUNK_SIZE, totalChars - charIndex);
      strncpy(chunk, &message[charIndex], chunkLen);
      chunk[chunkLen] = '\0';

      for (int i = 0; i < chunkLen; i++) {
        sendBinary(chunk[i]);
        Serial.print(" ");  // optional binary debug
        charsSinceResync++;

        if (charsSinceResync >= RESYNC_INTERVAL) {
          sendBinary(RESYNC_BYTE);
          sendBinary(RESYNC_BYTE);
          Serial.print(" RS RS ");  // optional debug
          charsSinceResync = 0;
        }
      }

      charIndex += chunkLen;
    }

    Serial.println("\n\nEnter another text message:");
  }
}

int readSerialInput(char* buffer, int maxLen) {
  int i = 0;
  while (Serial.available() && i < maxLen) {
    char c = Serial.read();
    if (c == '\n') break;
    buffer[i++] = c;
  }
  buffer[i] = '\0';
  // Flush remaining input if too long
  while (Serial.available()) Serial.read();
  return i;
}

void appendTerminator(char* buf, int currentLen) {
  if (currentLen + 2 >= MAX_TEXT_LENGTH) return;  // prevent overflow
  buf[currentLen++] = '~';
  buf[currentLen++] = '*';
  buf[currentLen] = '\0';
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    sendBit((c >> i) & 1);
  }
}

void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
<<<<<<< HEAD
  while (micros() - cycle < sendSpeed) {
    yield();
  }
=======
  while (micros() - cycle < sendSpeed);
>>>>>>> parent of 3cd5897 (Merge branch 'main' of https://github.com/JuiceBoxNerd/FSO-Project)
  cycle = micros();
  Serial.print(x);  // optional binary print
}

void startSignal() {
  digitalWrite(transmitter, HIGH);
  delay(startBuffer);
  digitalWrite(transmitter, LOW);
  delay(startBuffer);
  cycle = micros();
}
