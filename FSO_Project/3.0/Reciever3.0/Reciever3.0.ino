const int recSpeed = 25000;
const int startBuffer = 500;
const int receiver = 2;
long cycle = micros();
const int tolerance = 3;
const int bufferTolerance = 10;
String binaryInput = "";
String text = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(receiver, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Ready to receive.");
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
  int endPrimed = 0;
  while (true) {
    yield();
    if(endPrimed < 0){
      endPrimed = 0;
    }
    binaryInput = binaryInput + getBit(binaryInput);
    while(binaryInput.length() >= 8) {
      yield();
      String byteCandidate = binaryInput.substring(0, 8);

      if (byteCandidate == "01111110"){
        endPrimed = 2;
      }
      if (byteCandidate == "00101010" && endPrimed > 1){
        break;
      }
      if (byteCandidate == "00000000") {
        stopCount++;
        if (stopCount >= 20) break;
      } else {
        stopCount = 0;
      }
      endPrimed--;
      char decodedChar = binaryToChar(byteCandidate);
      text += decodedChar;
      binaryInput = binaryInput.substring(8);
    }
  }
  Serial.println();
}


boolean startSignal() {
  cycle = micros();
  while(digitalRead(receiver)){
    yield();
    if ((micros() - cycle) >= 500000){
      Serial.println("Start Failure");
      break;
    }
  }
  return (micros()-cycle >= (startBuffer*(bufferTolerance-1)/bufferTolerance));
}

String getBit(String input) {
  cycle = micros();
  int zcount = 0;
  int ocount = 0;
  String output = "";
  while(!digitalRead(receiver)){
    yield();
    if((micros()-cycle) > (recSpeed*170)){
      Serial.print("Broke 0s");
      break;
    }
  }
  if((micros()-cycle) > (recSpeed*((tolerance-1)/tolerance))){
    zcount = ((micros()-cycle)+(recSpeed*(1/tolerance)))/recSpeed;
    for(int i = 1; i <= zcount; i++){
      output = output + "0";
      Serial.print(output);
    }
    return output;
  }
  cycle = micros();
  while(digitalRead(receiver)){
    yield();
    if((micros()-cycle) > (recSpeed*170)){
      Serial.print("Broke 1s");
      break;
    }
  }
  if((micros()-cycle) > (recSpeed*((tolerance-1)/tolerance))){
    ocount = ((micros()-cycle)+(recSpeed*(1/tolerance)))/recSpeed;
    for(int i = 1; i <= ocount; i++){
      output = output + "1";
      Serial.print(output);
    }
    return output;
  }
  return output;
}




