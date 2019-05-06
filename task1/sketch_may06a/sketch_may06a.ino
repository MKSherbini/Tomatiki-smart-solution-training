unsigned long timer = 0;
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  if (millis() - timer > 2000) {
    digitalWrite(LED_BUILTIN, LOW);
    timer = millis();
  }
  else if (millis() - timer > 1000) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  delay(10);
}
