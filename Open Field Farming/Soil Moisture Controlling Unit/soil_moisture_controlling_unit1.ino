
#include <LiquidCrystal.h>

const int LM35 = A0;
const int NRF = 13;
const int LedRed = 12;
const int LedGreen = 11;


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Soil Moisture");
  lcd.setCursor(0,1);
  lcd.print("Control Unit");
  pinMode(NRF, OUTPUT);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  delay(2000);
  lcd.clear();
  lcd.print("Temp = ");
  lcd.setCursor(0,1);
  lcd.print("Moisture = ");
}

void loop() {

  int value = analogRead(LM35);
  float Temperature = value * 500.0 / 1024.0;
  lcd.setCursor(7,0);
  lcd.print(Temperature); 
  lcd.setCursor(11,1);
  

  if (Temperature > 50){
    digitalWrite(NRF, HIGH);
    digitalWrite(LedRed, HIGH);
    digitalWrite(LedGreen, LOW);
    lcd.print("LOW ");
  }
  else {
    digitalWrite(NRF, LOW);
    digitalWrite(LedRed, LOW);
    digitalWrite(LedGreen, HIGH);
    lcd.print("HIGH ");
  }
  
   delay(1000);
}