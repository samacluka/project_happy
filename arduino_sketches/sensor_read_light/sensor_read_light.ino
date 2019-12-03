#define LEDPIN 11 //LED brightness (PWM) writing
#define LIGHTSENSORPIN A1 //Ambient light sensor reading

void setup() {
    pinMode(LIGHTSENSORPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    float reading = analogRead(LIGHTSENSORPIN); //Read light level
    float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
    Serial.println(square_ratio);
    square_ratio = pow(square_ratio, 2.0); //Square to make response more obvious
    
    analogWrite(LEDPIN, 255.0 * square_ratio); //Adjust LED brightness relatively
    Serial.println(reading);
    Serial.println(square_ratio); //Display reading in serial monitor
    Serial.println("\n");
    delay(500);
}
