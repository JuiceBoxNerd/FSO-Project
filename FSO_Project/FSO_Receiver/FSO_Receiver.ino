<<<<<<< HEAD
const int recSpeed = 50;
=======

const unsigned long recSpeed = 50000UL; //50,000 microseconds
>>>>>>> parent of 932450e (Revert)
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;
<<<<<<< HEAD
long cycle = millis();
=======
unsigned long cycle = 0;
>>>>>>> parent of 932450e (Revert)

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
<<<<<<< HEAD
    Serial.println();
=======
>>>>>>> parent of 932450e (Revert)
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
<<<<<<< HEAD
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
=======
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
>>>>>>> parent of 932450e (Revert)
    }
  }
  Serial.println();
}

<<<<<<< HEAD
boolean bitStart(){
  for(int i = 0; i < 50; i++){
    if(analogRead(receiver) > threshold){
      return false;
    }
    delay(10);
  }
  delay(recSpeed/2 + 500);
  cycle = millis();
=======
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
>>>>>>> parent of 932450e (Revert)
  return true;
}


<<<<<<< HEAD
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

String getBit(String input) {
  int samples = 10;
=======


/*String getBit(String input) {
  const int samples = 15;
>>>>>>> parent of 932450e (Revert)
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

  cycle = cycle + 50;
  return output;
<<<<<<< HEAD
}
=======
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
>>>>>>> parent of 932450e (Revert)
