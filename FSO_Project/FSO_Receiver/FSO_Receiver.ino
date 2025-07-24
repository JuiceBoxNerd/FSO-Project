
const int recSpeed = 25;
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";
String text = "";
int spaceCount = 0;


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

boolean bitStart(){
  for(int i = 0; i < 50; i++){
    if(analogRead(receiver) > threshold){
      return false;
    }
    delay(10);
  }
  delay(recSpeed/2 + 500);
  return true;
}


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
  unsigned long start = millis();
  int samples = 5;
  int lightDetectedCount = 0;

  // Sample multiple times during first part of bit window
  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetectedCount++;
    }
    delay((recSpeed / 10) / samples);  // spread samples evenly
  }

  // Wait out the rest of the bit window
  while (millis() - start < recSpeed) {}

  bool detected = (lightDetectedCount > samples / 2);  // majority vote

  String newBit = detected ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
    spaceCount++;
  }

  if (spaceCount >= 160) {
    Serial.println();
    spaceCount = 0;
  }

  return output;
}

