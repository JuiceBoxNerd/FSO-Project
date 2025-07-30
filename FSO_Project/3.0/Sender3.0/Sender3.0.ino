const int sendSpeed = 25;
const int startBuffer = 500;
const int transmitter = 2;
long cycle = millis();

void setup() {
  // put your setup code here, to run once:
  pinMode(transmitter, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Enter a text message:");
}

void loop() {
  if (Serial.available()) {
    String inputText = Serial.readStringUntil('\n');
    Serial.print("Transmitting: " + inputText + "\nBinary Output: ");
    inputText += ">";  // Use '>' as terminator
    startSignal();

    for (int i = 0; i < inputText.length(); i++) {
      char c = inputText[i];
      sendBinary(c);
      Serial.print(" ");
    }
    Serial.println("\n\nEnter another text message:");
  }
}

void sendBinary(char c) {
  for (int i = 7; i >= 0; i--) {
    sendBit((c >> i) & 1);
  }
}


void sendBit(int x) {
  digitalWrite(transmitter, x ? HIGH : LOW);
  while (millis() - cycle < sendSpeed);
  cycle = millis();
  Serial.print(x);
}

void startSignal() {
  digitalWrite(transmitter, HIGH);
  delay(startBuffer);
  digitalWrite(transmitter, LOW);
  delay(startBuffer);
  cycle = millis();
}