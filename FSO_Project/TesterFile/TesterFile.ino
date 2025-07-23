String binaryInput = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
}

void loop() {
  // put your main  code here, to run repeatedly:
  digitalWrite(2, HIGH);
  if(Serial.readStringUntil('\n') == "stop"){
    digitalWrite(2, LOW);
  }
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