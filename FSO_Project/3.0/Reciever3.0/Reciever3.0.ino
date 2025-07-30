const int recSpeed = 25;
const int startBuffer = 500;
const int receiver = 2;
long cycle = millis();
const int tolerance = 3;
const int bufferTolerance = 10;
String binaryInput = "";
String text = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println();
}

void loop() {
  if(startSignal()){
    delay(startBuffer);
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + text);
    Serial.println();
  }
  binaryInput = "";
  text = "";
}

char binaryToChar(String byteStr) {
  return (char)strtol(byteStr.c_str(), NULL, 2);
}

void getInput() {
  int stopCount = 0;
  while (true) {
    binaryInput = binaryInput + getBit(binaryInput);
    while(binaryInput.length() >= 8) {
      String byteCandidate = binaryInput.substring(0, 8);

      if (byteCandidate == "00111110") break; // '>' terminator
      if (byteCandidate == "00000000") {
        stopCount++;
        if (stopCount >= 20) break;
      } else {
        stopCount = 0;
      }

      char decodedChar = binaryToChar(byteCandidate);
      text += decodedChar;
      binaryInput = binaryInput.substring(8);
    }
  }
  Serial.println();
}


boolean startSignal() {
  cycle = millis();
  while(digitalRead(receiver)){}
  return (millis()-cycle >= (startBuffer*(bufferTolerance-1)/bufferTolerance));
}

String getBit(String input) {
  cycle = millis();
  int zcount = 0;
  int ocount = 0;
  String output = "";
  while(!digitalRead(receiver)){
    if((millis()-cycle) > (recSpeed*170)){
      break;
    }
  }
  if((millis()-cycle) > (recSpeed*((tolerance-1)/tolerance))){
    zcount = ((millis()-cycle)+(recSpeed*(1/tolerance)))/recSpeed;
    for(int i = 1; i <= zcount; i++){
      output = output + "0";
    }
    return output;
  }
  cycle = millis();
  while(digitalRead(receiver));
  if((millis()-cycle) > (recSpeed*((tolerance-1)/tolerance))){
    ocount = ((millis()-cycle)+(recSpeed*(1/tolerance)))/recSpeed;
    for(int i = 1; i <= ocount; i++){
      output = output + "1";
    }
    return output;
  }
  return output;
}




