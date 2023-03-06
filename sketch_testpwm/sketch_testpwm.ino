
const int Pin = 15; 

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  int dutyCycle=0;
   //increase the LED brightness
  for(int dutyCycle = 0; dutyCycle < 255; dutyCycle++){   
    // changing the LED brightness with PWM
    analogWrite(Pin, dutyCycle);
    Serial.println(dutyCycle);
    delay(50);

  }

   //decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle > 0; dutyCycle--){
    // changing the LED brightness with PWM
    analogWrite(Pin, dutyCycle);
    Serial.println(dutyCycle);
    delay(50);
  }
}