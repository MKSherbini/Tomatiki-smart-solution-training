unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
}

void loop() {

  if (millis() - timer > 2000) {
    digitalWrite(2, LOW);
    timer = millis();
  }
  else if (millis() - timer > 1000) {
    digitalWrite(2, HIGH);
  }


  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  delay(10);
}
