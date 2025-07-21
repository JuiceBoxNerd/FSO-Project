void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Enter a text message:")

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Binary Output: ");
    
    for(int i = 0; i < inputText.length(); i++){
      char c = inputText[i];
      printBinary(c);
      Serial.print(" ");
      }
    Serial.println();
    Serial.println("Enter another text message:");
}
void printBinary(char c) {
  for(int i = 7; i >= 0; i--){
    if(c & (1 << i)){
      Serial.print("1");
    }
    else{
      Serial.print("0");
    }
  }
}
