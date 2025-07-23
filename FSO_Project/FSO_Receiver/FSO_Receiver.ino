
const int recSpeed = 100;
const int receiver = A1;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";


void setup() {
  // put your setup code here, to run once:
  pinMode

  Serial.begin(9600);
  threshold = initializer();
  Serial.println("Threshold: " + threshold);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bitStart()){
    Serial.println("Receiving code...");
    getInput();
    Serial.println("Decoded word is " + binaryToText());
  }
}

int initializer(){
  unsigned long start = millis();
  int total = 0;
  int count = 0;
  while(millis() - start < 1000){
    total += analogRead(receiver);
    count++;
  }
  return total/count;
}

String binaryToText(){
  String text = "";
  int len = binaryInput.length();
  for(int i = 0; i < len; i += 8){
    String letter = binaryInput.substring(i, i+8);
    char c = strtol(letter.cstr(), NULL, 2);
    Serial.print(c);
    text += c;
  }
  Serial.println();
  return text;
}

void getInput(){
  int i = 0
  while(i == 0){
    binaryInput = getBit(binaryInput);
    if(binaryInput.length() % 8 == 0){
      if(binaryInput.substring(binaryInput.length() - 8) == "00111110"){
        i = 1;
      }
      else if(binaryInput.substring(binaryInput.length() - 8) == "00000000"){
        i = 1;
      }
    }
  }
}

boolean bitStart(){
  for(int i = 0; i < 50; i++){
    if(analogRead(receiver) < threshold){
      return false;
    }
    delay(10);
  }
  return true;
}

String getBit(String input){
  String output = input;
  unsigned long start = millis();
  while(millis() - start < recSpeed){
    if(analogRead(receiver) >= threshold){
      delay(recSpeed - (millis() - start));
      return output + "1";
    }
    
  return output + "0";
}

