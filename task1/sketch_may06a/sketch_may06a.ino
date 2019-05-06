long timer = 0;
void setup() {
  Serial.begin(115200);
<<<<<<< HEAD
  pinMode(2, OUTPUT);
=======
  pinMode(LED_BUILTIN, OUTPUT);
>>>>>>> 81c1c98c67bb4883e50183e27b31d7fdb3c50634
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
