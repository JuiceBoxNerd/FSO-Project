#include <Wire.h>
const int sendSpeed = 50;
const int transmitter = 2;

long cycle = millis();
int bitsSentSinceResync = 0;
const int RESYNC_INTERVAL = 64;  // Insert resync bit every 64 bits

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
  if(Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting " + inputText + " to Binary...\nBinary Output: ");
    inputText = inputText + ">";
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
    // Before sending each bit, check if it’s time to send a resync bit
    if(bitsSentSinceResync >= RESYNC_INTERVAL){
      sendResyncBit();
      bitsSentSinceResync = 0;
    }

    if(c & (1 << i)){
      Serial.print("1");
      sendBit(1);
    }
    else{
      Serial.print("0");
      sendBit(0);
    }
    bitsSentSinceResync++;
  }
}

// Send a special resync bit, here using '1' as the marker
void sendResyncBit() {
  Serial.print("[R]");  // Indicate resync bit in serial output
  sendBit(1);           // Send '1' as resync bit
}

void sendBit(int x){
  if(x == 1){
    digitalWrite(transmitter, HIGH);
    while(millis()-cycle < sendSpeed);
  }
  else{
    digitalWrite(transmitter, LOW);
    while(millis()-cycle < sendSpeed);
  }
  cycle = millis();
}

void startSignal(){
  Wire.beginTransmission(4);
  Wire.write('S');
  delay(500);
  Wire.endTransmission();
  delay(500);
  cycle = millis();
}

/*void startBit(){
  digitalWrite(transmitter, HIGH);
  delay(500);
  digitalWrite(transmitter, LOW);
  delay(500);
  cycle = millis();
}
*/