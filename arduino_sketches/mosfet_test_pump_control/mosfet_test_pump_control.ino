#define pumpPin A2

void setup(){
  pinMode(pumpPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  analogWrite(pumpPin, 255); 
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000); 
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  analogWrite(pumpPin, 0);
  delay(1000);
}
