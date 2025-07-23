String binaryInput = "";

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main  code here, to run repeatedly:
  Serial.begin(9600)
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