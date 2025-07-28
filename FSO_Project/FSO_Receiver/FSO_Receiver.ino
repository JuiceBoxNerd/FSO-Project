#include <Wire.h>
volatile bool startReceiving = false;
const int recSpeed = 50;
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;
long cycle = millis();
void receiveEvent(int howMany);

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  // put your setup code here, to run once:
  pinMode(receiver, INPUT);

  Serial.begin(9600);
  while(!Serial);
  delay(500);
  threshold = initializer();
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

  if(startSignal()){
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text);
    Serial.println();
    startReceiving = false;
  }
  binaryInput = "";
  text = "";
  
}

int initializer() {
  unsigned long start = millis();
  long total = 0;     // use long to avoid overflow
  int count = 0;

  while (millis() - start < 1000) {
    int reading = analogRead(receiver);
    total += reading;
    count++;

    // Optional: print raw readings to check range
    Serial.print("Reading: ");
    Serial.print(reading);
    Serial.print(" | Count: ");
    Serial.print(count);
    Serial.print(" | Total: ");
    Serial.print(total);
    Serial.print(" | Current Avg: ");
    Serial.println(total / count);
  }

  int average = total / count;
  Serial.print("Final Threshold: ");
  Serial.println(average - 400);
  return average - 400;
}

char binaryToChar(String byteStr) {
  // Convert 8-bit binary string to char
  return (char)strtol(byteStr.c_str(), NULL, 2);
}


void getInput() {
  while (true) {
    binaryInput = getBit(binaryInput);

    if (binaryInput.length() % 8 == 0) {
      String lastByte = binaryInput.substring(binaryInput.length() - 8);
      if (lastByte == "00111110" || lastByte == "00000000") {
        break;  // terminator found
      }

      // Decode current byte and append to text
      String currentByte = binaryInput.substring(0, 8);
      text += binaryToChar(currentByte);
      binaryInput = "";
    }
  }
  Serial.println();
}

void receiveEvent(int howMany){
  while(Wire.available()){
    char cmd = Wire.read();
    if(cmd == 's' || cmd == 'S'){
      delay(recSpeed/2 + 500);
      startReceiving = true;
    }
  }

}

boolean startSignal() {
  if (!startReceiving) return false;

  // mimic old timing: delay for 500ms of "dark", then sync

  delay(recSpeed / 2 + 500); // match previous timing
  cycle = millis();
  return true;
}

/*boolean bitStart(){
  for(int i = 0; i < 50; i++){
    if(analogRead(receiver) > threshold){
      return false;
    }
    delay(10);
  }
  delay(recSpeed/5 + 500);
  cycle = millis();
  return true;
}
*/


/*String getBit(String input) {
  unsigned long start = millis();
  bool detected = false;

  // Only check for laser flash in first portion of the bit window
  while (millis() - start < recSpeed / 6) {
    if (analogRead(receiver) <= threshold) {
      detected = true;
      break;
    }
  }

  // Wait out the rest of the bit window
  while (millis() - start < recSpeed) {
    // Do nothing
  }

  // Add new bit to the string
  String newBit = detected ? "1" : "0";
  String output = input + newBit;
  
  
  
  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
  }
  spaceCount++;

  // After 160 spaces (~1280 bits), print newline and reset counter
  if (spaceCount >= 160) {
    Serial.println();
    spaceCount = 0;
  }

  return output;
}
*/

const int RESYNC_INTERVAL = 64;  // Number of bits after which resync is expected
int bitsSinceLastResync = 0;

String getBit(String input) {
  int samples = 10;
  int lightDetectedCount = 0;

  // Sample multiple times during first part of bit window
  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetectedCount++;
    }
    delay((recSpeed / 5) / samples);  // spread samples evenly
  }

  // Wait out the rest of the bit window
  while (millis() - cycle < recSpeed);
  cycle += recSpeed;
  bool detected = (lightDetectedCount > samples / 2);  // majority vote

  String newBit = detected ? "1" : "0";
  String output = input + newBit;

  bitsSinceLastResync++;

  // Check for resync pattern (for example: a single '1' bit as resync every 64 bits)
  if (bitsSinceLastResync >= RESYNC_INTERVAL) {
    // If this bit is '1', reset timing to resync
    if (detected) {
      cycle = millis();  // Resync timing to current time
      Serial.println("\n*** Resync performed ***\n");
    }
    bitsSinceLastResync = 0;  // Reset counter regardless
  }

  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
    spaceCount++;  // count *groups* of 8 bits
  }

  // After 20 groups, print a newline and reset counter
  if (spaceCount >= 20) {
    Serial.println();
    spaceCount = 0;  // reset counter!
  }

<<<<<<< Updated upstream
=======
  cycle = cycle + recSpeed;
>>>>>>> Stashed changes
  return output;
}
