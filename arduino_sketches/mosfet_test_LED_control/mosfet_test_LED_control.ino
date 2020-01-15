#define fadePin A3

void setup(){
  pinMode(fadePin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  analogWrite(fadePin, 255); 
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000); 
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  analogWrite(fadePin, 0);
  delay(1000);
}
