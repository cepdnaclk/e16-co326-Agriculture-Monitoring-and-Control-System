int gas_sensor = A0; 
int flame_sensor = 4;
int flame;
int gas;
int randNumber;


void setup() {
  
  pinMode(gas_sensor, INPUT);
  pinMode(flame_sensor, INPUT);
  Serial.begin(9600);
}


void loop() {
  
  flame = digitalRead(flame_sensor);
  gas = analogRead(gas_sensor);
  
  Serial.print("Flame sensor:");
  Serial.print(flame);
  
  Serial.print(", ");

  Serial.print("Gas sensor:");

  if (gas > 400){
    Serial.println("1");
  }
  else {
    Serial.println("0");
  }
  delay(500);

}
