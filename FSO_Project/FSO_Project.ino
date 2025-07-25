
const unsigned long sendSpeed = 50000; //microseconds
const int transmitter = 2;
unsigned long nextBitTime - 0;
void setup() {
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
    startBit();
    
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
  for (int i = 7; i >= 0; i--) {
    bool bit = c & (1 << i);
    Serial.print(bit ? "1" : "0");
    sendBit(bit);
  }
}

void sendBit(bool bit) {
  digitalWrite(transmitter, bit ? HIGH : LOW);

  // Wait until full bit interval has passed
  unsigned long now = micros();
  if (nextBitTime > now) {
    delayMicroseconds(nextBitTime - now);
  }

  // Schedule next bit
  nextBitTime += sendSpeed;
}


void startBit() {
  // Send a HIGH signal for 500 ms (sync pulse)
  digitalWrite(transmitter, HIGH);
  unsigned long syncStart = micros();
  while (micros() - syncStart < 500000) {
    // wait for 500 ms
  }

  // Send a LOW signal for 500 ms (idle gap after sync)
  digitalWrite(transmitter, LOW);
  unsigned long gapStart = micros();
  while (micros() - gapStart < 500000) {
    // wait for another 500 ms
  }


