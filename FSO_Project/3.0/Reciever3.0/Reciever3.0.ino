<<<<<<< Updated upstream
const int recSpeed = 10000;         // microseconds per bit
const int startBuffer = 100;        // milliseconds
=======
const int recSpeed = 5000;
const int startBuffer = 500;
>>>>>>> Stashed changes
const int receiver = 2;
const int tolerance = 3;
const int bufferTolerance = 10;

#define MAX_BITS 1200               // bit buffer size (up to 150 chars)
#define MAX_TEXT 125                // decoded message size

char binaryInput[MAX_BITS + 1];
char text[MAX_TEXT + 1];
int bitIndex = 0;
int charIndex = 0;
unsigned long cycle = 0;

const char RESYNC_BYTE = 0xFF;      
const int RESYNC_INTERVAL = 25;

int charCount = 0;
bool expectingSecondResyncByte = false;

void setup() {
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  delay(200);
  while (!Serial);
  Serial.println();
}

void loop() {
  if (startSignal()) {
    delay(startBuffer);
    Serial.println("Receiving code...");
    getInput();
    text[charIndex] = '\0';  // null-terminate
    Serial.print("Decoded word is ");
    Serial.println(text);
    Serial.println();
  }

  bitIndex = 0;
  charIndex = 0;
  charCount = 0;
  expectingSecondResyncByte = false;
  binaryInput[0] = '\0';
  text[0] = '\0';
}

char binaryToChar(const char* byteStr) {
  return (char) strtol(byteStr, NULL, 2);
}

void getInput() {
  int stopCount = 0, endPrimed = 0;

  while (true) {
    yield();
    if (endPrimed < 0) endPrimed = 0;

    getBit();  // appends to binaryInput, updates bitIndex

    while (bitIndex >= 8) {
      char byteStr[9] = {0};
      memcpy(byteStr, binaryInput, 8);

      if (!strcmp(byteStr, "01111110")) endPrimed = 2;
      if (!strcmp(byteStr, "00101010") && endPrimed > 1) return;

      if (!strcmp(byteStr, "00000000")) {
        if (++stopCount >= 20) return;
      } else stopCount = 0;

      endPrimed--;

      char decodedChar = binaryToChar(byteStr);

      // Resync detection
      if (decodedChar == RESYNC_BYTE) {
        if (expectingSecondResyncByte) {
          Serial.println("Resync detected, realigning...");
          // Reset buffer and indexes to realign
          bitIndex = 0;
          charIndex = 0;
          charCount = 0;
          expectingSecondResyncByte = false;
          binaryInput[0] = '\0';
          text[0] = '\0';
          // Skip adding these resync chars to output
          memmove(binaryInput, binaryInput + 8, bitIndex - 8);  // Remove resync byte bits
          bitIndex -= 8;
          continue;  // go to next byte
        } else {
          expectingSecondResyncByte = true;
          // Remove first resync byte bits before next iteration
          memmove(binaryInput, binaryInput + 8, bitIndex - 8);
          bitIndex -= 8;
          continue;
        }
      } else {
        expectingSecondResyncByte = false;
      }

      if (charIndex < MAX_TEXT - 1) {
        text[charIndex++] = decodedChar;
      }

      charCount++;
      if (charCount >= RESYNC_INTERVAL) {
        charCount = 0;
      }

      // Shift remaining bits left
      memmove(binaryInput, binaryInput + 8, bitIndex - 8);
      bitIndex -= 8;
      binaryInput[bitIndex] = '\0';
    }
  }
}

bool startSignal() {
  // Wait for pin to go LOW (start signal)
  unsigned long startTime = micros();
  while (!digitalRead(receiver)) {
    if ((micros() - startTime) > (unsigned long)startBuffer * 1000) {
      Serial.println("Start Failure: 1");
      return false;
    }
    yield();
  }
  return (micros()-startTime) > (((unsigned long)startBuffer * 1000 * (bufferTolerance-1))/bufferTolerance);
}


<<<<<<< HEAD
void getBit() {
  unsigned long duration;
  int count;
=======
boolean startSignal() {
  cycle = micros();
  while(!digitalRead(receiver)){
    yield();
<<<<<<< Updated upstream
    if ((micros() - cycle)>= 10000000){
      Serial.print("Broke on start");
      break;
    }
  }
  return (micros()-cycle >= (startBuffer*(bufferTolerance-1)/bufferTolerance));
=======
    if ((micros() - cycle) >= (((unsigned long)startBuffer*1000*(bufferTolerance+1))/bufferTolerance)){
      Serial.println("Start Failure: " + digitalRead(receiver));
      break;
    }
  }
  return ((micros() - cycle) >= ((unsigned long) startBuffer*1000*(bufferTolerance-1)/bufferTolerance));
>>>>>>> Stashed changes
}
>>>>>>> parent of 3cd5897 (Merge branch 'main' of https://github.com/JuiceBoxNerd/FSO-Project)

  // Measure LOW
  cycle = micros();
<<<<<<< Updated upstream
  while (digitalRead(receiver)) {
    yield();
    if ((micros() - cycle) > (unsigned long)recSpeed * 4) {
=======
  int zcount = 0;
  int ocount = 0;
  String output = "";
  while(digitalRead(receiver)){
    yield();
    if((micros()-cycle) > ((unsigned long)recSpeed *160)){
>>>>>>> Stashed changes
      Serial.print("Broke 0s");
      return;
    }
  }

  duration = micros() - cycle;
  if (duration > ((unsigned long)recSpeed * (tolerance - 1)) / tolerance) {
    count = (duration + recSpeed / tolerance) / recSpeed;
    if (bitIndex + count < MAX_BITS) {
      for (int i = 0; i < count; i++) binaryInput[bitIndex++] = '0';
      binaryInput[bitIndex] = '\0';
      return;
    }
  }

  // Measure HIGH
  cycle = micros();
<<<<<<< Updated upstream
  while (!digitalRead(receiver)) {
    yield();
    if ((micros() - cycle) > (unsigned long)recSpeed * 4) {
      return;
=======
  while(!digitalRead(receiver)){
    yield();
    if((micros()-cycle) > ((unsigned long)recSpeed *160)){
      Serial.print("Broke 1s");
      break;
>>>>>>> Stashed changes
    }
  }

  duration = micros() - cycle;
  if (duration > ((unsigned long)recSpeed * (tolerance - 1)) / tolerance) {
    count = (duration + recSpeed / tolerance) / recSpeed;
    if (bitIndex + count < MAX_BITS) {
      for (int i = 0; i < count; i++) binaryInput[bitIndex++] = '1';
      binaryInput[bitIndex] = '\0';
    }
  }
}
