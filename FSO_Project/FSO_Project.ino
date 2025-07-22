#
int sendSpeed = 100;
int transmitter = D2;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Enter a text message:");

}


void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    inputText + ">";
    startBit();
    Serial.print("Transmitting Binary...\nBinary Output: ");
    
    for(int i = 0; i < inputText.length(); i++){
      char c = inputText[i];
      sendBinary(c);
      Serial.print(" ");
      }
    Serial.println();
    Serial.println("Enter another text message:");
  }
}



void sendBinary(char c) {
  for(int i = 7; i >= 0; i--){
    if(c & (1 << i)){
      Serial.print("1");
      sendBit(1);
    }
    else{
      Serial.print("0");
      sendBit(0);
    }
  }
}

void sendBit(int x){
  if(x == 1){
    digitalWrite(transmitter, HIGH)
  }
  else{
    digitalWrite(transmitter, LOW)
  }
  delay(sendSpeed);
}

void startBit(){
  digitalWrite(transmitter, HIGH);
  delay(500);
}


