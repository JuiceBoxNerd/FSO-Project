
int recSpeed = 100;
int receiver = A1;
int threshold = 100;
int bitCount = 0;
String binaryInput = "";


void setup() {
  // put your setup code here, to run once:
  pinMode

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bitStart()){
    getBit()
  }
}

void binaryToText(){
  int len = binaryInput.length();
  for(int i = 0; i < len; i += 8){
    String letter = binaryInput.substring(i, i+8);
  }
}

void getInput(){
  int i = 0
  while(i = 1){
    getBit();
    if(binaryInput.length % 8 == 0){
      if(binaryInput.substring(binaryInput.length() - 8) == "00111110"){
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

void getBit(){
  if(analogRead(receiver) >= threshold){
    binaryInput += "1";
  }
  else{
    binaryInput += "0";
  }
}

