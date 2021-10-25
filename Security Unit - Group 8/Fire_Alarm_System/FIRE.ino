#include <LiquidCrystal.h>
int sensor_pin = 9;
const int rs = 12, en = 13, d4 =A3 , d5 = A2, d6 = A1, d7 = A0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {
  // put your setup code here, to run once:
  pinMode(4,OUTPUT);
  pinMode(9,INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(2,0);
  lcd.print("  welcome to ");
  lcd.setCursor(4,1);
  lcd.print("Security Unit");
  delay(1000);
     lcd.clear();
      for (int positionCounter = 0; positionCounter < 30; positionCounter++) {
    // scroll one position right:
      lcd.setCursor(16,0);
      lcd.print("by ");
      lcd.setCursor(16,1);
      lcd.print("Group 08 ");
      lcd.scrollDisplayLeft();
      delay(5);
      pinMode(4,OUTPUT);
      pinMode(5,OUTPUT);
      pinMode(6,OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
 FIRE();
}
 void FIRE()
{
   
   if(digitalRead(sensor_pin)==HIGH){
      Serial.println("fire:detected ");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("FIRE :");
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
      digitalWrite(6,HIGH);
      lcd.setCursor(5,1);
      lcd.print("detected ");
     }
     else{
      Serial.println("safe");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("FIRE :");
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
      digitalWrite(6,LOW);
      lcd.setCursor(5,1);
      lcd.print("safe");
     }
   delay(1000);
}
