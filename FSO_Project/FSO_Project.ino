#include <Wire.h>
const int sendSpeed = 50;
const int transmitter = 2;

long cycle = millis();
void setup() {
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println();
  Serial.println("Enter a text message:");

}


void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting " + inputText + " to Binary...\nBinary Output: ");
    inputText = inputText + ">";
    //startBit();
    startSignal();
    
    for(int i = 0; i < inputText.length(); i++){
      char c = inputText[i];
      sendBinary(c);
      Serial.print(" ");
      }
    Serial.println("\n\n");
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
  digitalWrite(transmitter, x ? HIGH:LOW);
  while(millis()-cycle < sendSpeed);
  cycle += sendSpeed;
}

void startSignal(){
  Wire.beginTransmission(4);
  Wire.write("s");
  Wire.endTransmission();
  delay(500);

}
/*void startBit(){
  digitalWrite(transmitter, HIGH);
  delay(500);
  digitalWrite(transmitter, LOW);
  delay(500);
  cycle = millis();
}
*/