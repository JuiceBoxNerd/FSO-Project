
const int recSpeed = 1000;
const int receiver = A3;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";


void setup() {
  // put your setup code here, to run once:
  pinMode(receiver, INPUT);

  Serial.begin(9600);
  while(!Serial);
  delay(500);
  threshold = initializer();
}

void loop() {
  // put your main code here, to run repeatedly:

  if(bitStart()){
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + binaryToText());
  }
  binaryInput = "";
  
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
  Serial.println(average - 100);
  return average - 100;
}

String binaryToText(){
  String text = "";
  int len = binaryInput.length();
  for(int i = 0; i < len; i += 8){
    String letter = binaryInput.substring(i, i+8);
    char c = strtol(letter.c_str(), NULL, 2);
    Serial.print(c);
    text += c;
  }
  Serial.println();
  return text;
}

void getInput() {
  while (true) {
    binaryInput = getBit(binaryInput);

    if (binaryInput.length() % 8 == 0) {
      String lastByte = binaryInput.substring(binaryInput.length() - 8);
      if (lastByte == "00111110" || lastByte == "00000000") {
        break;
      }
    }
  }
}

boolean bitStart(){
  for(int i = 0; i < 50; i++){
    if(analogRead(receiver) > threshold){
      return false;
    }
    delay(10);
  }
  delay(recSpeed/2);
  return true;
}

String getBit(String input) {
  unsigned long start = millis();

  while (millis() - start < recSpeed) {
    if (analogRead(receiver) <= threshold) {
      Serial.println(input + "1");
      delay(recSpeed - (millis() - start));  // wait remaining bit duration
      return input + "1";
    }
  }

  Serial.println(input + "0");
  return input + "0";
}

