  // Sample multiple times during first part of bit window
  for (int i = 0; i < samples; i++) {
    if (analogRead(receiver) <= threshold) {
      lightDetectedCount++;
    }
    delay((recSpeed / 12) / samples);  // spread samples evenly
  }

  // Wait out the rest of the bit window
  while (millis() - cycle < recSpeed) {}

  bool detected = (lightDetectedCount > samples / 2);  // majority vote

  String newBit = detected ? "1" : "0";
  String output = input + newBit;

  Serial.print(newBit);
  if (output.length() % 8 == 0) {
    Serial.print(" ");
    spaceCount++;  // count *groups* of 8 bits
  }

  // After 20 groups (you can change this), print a newline and reset counter
  if (spaceCount >= 20) {
    Serial.println();
    spaceCount = 0;  // reset counter!
  }

  cycle = cycle  + recSpeed;
  return output;
}
