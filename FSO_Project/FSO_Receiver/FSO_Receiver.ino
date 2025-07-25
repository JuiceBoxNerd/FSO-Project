
const unsigned long recSpeed = 50000UL; //50,000 microseconds
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;
unsigned long cycle = 0;

void setup() {
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

  if(bitStart()){
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text);
    Serial.println();
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
    binaryInput = getManchesterBit(binaryInput);

    if (binaryInput.length() >= 8) {
      String currentByte = binaryInput.substring(0, 8);
      binaryInput = binaryInput.substring(8);  // remove processed byte

      if (currentByte == "00111110" || currentByte == "00000000") {
        break;  // end of message
      }

      char decodedChar = binaryToChar(currentByte);
      if (decodedChar >= 32 && decodedChar <= 126) {
        text += decodedChar;
      } else {
        Serial.print(" [Invalid byte: ");
        Serial.print(currentByte);
        Serial.print("] ");
      }
    }
  }
  Serial.println();
}

boolean bitStart() {
  unsigned long startMicros = micros();
  int duration = 0;

  // Wait for the line to be LOW for ~500ms total
  while (micros() - startMicros < 500000UL){
    if (analogRead(receiver) > threshold) {
      return false;  // light detected too early, abort
    }
  }
  startMicros = micros();
  while(micros()-startMicros == 500000UL + (recSpeed / 3));
  cycle = micros();  // sync for bit timing
  return true;
}




/*String getBit(String input) {
  const int samples = 15;
  int lightDetectedCount = 0;

  // Sample multiple times during first part of bit window
  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetectedCount++;
    }
    delay((recSpeed / 12) / samples);  // spread samples evenly
  }

  // Wait out the rest of the bit window
  while (micros() - cycle < recSpeed) {}

  bool detected = (lightDetectedCount > samples / 2);  // majority vote

  String newBit = detected ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
    spaceCount++;  // count *groups* of 8 bits
  }

  // After 20 groups (you can change this), print a newline and reset counter
  if (spaceCount >= 20) {
    Serial.println();
    spaceCount = 0;  // reset counter!
  }

  cycle = cycle  + recSpeed;
  return output;
}
*/

String getManchesterBit(String input) {
  unsigned long halfBitDuration = recSpeed / 2;  // convert ms to us and half

  // Wait half bit interval, sample initial state
  while (micros() < cycle + halfBitDuration) {}
  int firstSample = analogRead(receiver) <= threshold ? LOW : HIGH;

  // Wait another half bit interval, sample again
  while (micros() < cycle + recSpeed) {}
  int secondSample = analogRead(receiver) <= threshold ? LOW : HIGH;

  // Decode based on transition
  int bitValue;
  if (firstSample == LOW && secondSample == HIGH) bitValue = 0;
  else if (firstSample == HIGH && secondSample == LOW) bitValue = 1;
  else {
    // No valid Manchester transition detected - treat as error or repeat sampling
    Serial.print("?");  // For debug
    cycle += recSpeed;
    return input + "?";  // Skip adding bit
  }

  // Append new bit
  input += String(bitValue);

  Serial.print(bitValue);
  if (input.length() % 8 == 0) Serial.print(" ");

  cycle += recSpeed;
  return input;
}
