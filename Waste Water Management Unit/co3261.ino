// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8,9,10,11,12,13);


#define LED_TEMP 7
#define LED_PH 6
#define LED_EC 5
#define LED_TANK_LEVEL 4

#define LED_VALVE_ON 3
#define LED_VALVE_OFF 2

#define INPUT_TEMP A0
#define INPUT_PH A1
#define INPUT_EC A2
#define INPUT_TANK_LEVEL A3

int treshold_temp = 40;

int temp_celcius = 0;
int ph_value = 0;
int e_conductivity = 0;
int tank_level = 0;

bool ishightemp = false;
bool isnotPHrange = false;
bool isnotECrange = false;
bool istankFIll = false;


void setup()
{
  
  
  pinMode(INPUT_TEMP, INPUT);
  pinMode(INPUT_TANK_LEVEL, INPUT);
  pinMode(INPUT_PH, INPUT);
  pinMode(INPUT_EC, INPUT);
  Serial.begin(9600);

  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_TANK_LEVEL, OUTPUT);
  pinMode(LED_PH, OUTPUT);
  pinMode(LED_EC, OUTPUT);
  pinMode(LED_VALVE_ON, OUTPUT);
  pinMode(LED_VALVE_OFF, OUTPUT);
  
  lcd.begin(16, 2);
  lcd.clear();
}


void loop() {
  // check the temperature
  temp_celcius = map(analogRead(INPUT_TEMP), 0, 1023, 0, 100);
  Serial.print(temp_celcius);
  Serial.print(" C ");
  
  if(temp_celcius < treshold_temp){
    digitalWrite(LED_TEMP, LOW);
    ishightemp = false;
  }
  	
  if(temp_celcius >= treshold_temp){
    digitalWrite(LED_TEMP, HIGH);
    ishightemp = true;
  }
  
  lcd.setCursor(0,0); // Sets the cursor to col 0 and row 0
  lcd.print("temp: "); // Prints Sensor Val: to LCD
  lcd.print(temp_celcius); // Prints value on PH value to LCD
  
  
  if(temp_celcius < 10)
  	{ lcd.print(" "); }
  
  
  // check the PH LEVEL
  ph_value = map(analogRead(INPUT_PH) , 0, 1023, 1, 14);
  //Serial.print(ph_value);
  //Serial.print(" PH ");
  
  if(ph_value > 8.5){
    digitalWrite(LED_PH, HIGH);
    isnotPHrange = true;
  }
  	
  if(ph_value < 4.5){
    digitalWrite(LED_PH, HIGH);
    isnotPHrange = true;
  }
  
  if(ph_value >= 4.5 && ph_value <= 8.5){
    digitalWrite(LED_PH, LOW);
    isnotPHrange = false;
  }
  
  
  lcd.setCursor(9,0); // Sets the cursor to col 0 and row 0
  lcd.print("pH: "); // Prints Sensor Val: to LCD
  lcd.print(ph_value); // Prints value on PH value to LCD
  if(ph_value < 10)
  	{ lcd.print(" "); }
  
  
  
  // check the Electrical Conductivity
  e_conductivity = map(analogRead(INPUT_EC) , 0, 1023, 1, 10);
  //Serial.print(e_conductivity);
  //Serial.print(" siemens per meter ");
  
  if(e_conductivity > 4){
    digitalWrite(LED_EC, HIGH);
    isnotECrange = true;
  }
  	
  if(e_conductivity <= 4){
    digitalWrite(LED_EC, LOW);
    isnotECrange = false;
  }
  
  lcd.setCursor(0,1); // Sets the cursor to col 0 and row 0
  lcd.print("EC: "); // Prints Sensor Val: to LCD
  lcd.print(e_conductivity); // Prints value on PH value to LCD
  if(e_conductivity < 10)
  	{ lcd.print(" "); }
  
  // check the Tank Water Level
  tank_level = map(analogRead(INPUT_TANK_LEVEL) , 0, 1023, 1, 100);
  //Serial.print(tank_level);
  //Serial.print(" Liter ");
  
  if(tank_level >= 75){
    digitalWrite(LED_TANK_LEVEL, HIGH);
    istankFIll = true;
  }
  	
  if(tank_level < 75){
    digitalWrite(LED_TANK_LEVEL, LOW);
    istankFIll= false;
  }
  
  	lcd.setCursor(7,1); // Sets the cursor to col 0 and row 0
  	lcd.print("TL: "); // Prints Sensor Val: to LCD
  	lcd.print(tank_level); // Prints value on PH value to LCD
  	if(tank_level < 10 || tank_level <100)
  		{ lcd.print(" "); }
 
  
  //Oprn the vales
  
  if(ishightemp || isnotPHrange || isnotECrange){
    digitalWrite(LED_VALVE_ON, HIGH);
    digitalWrite(LED_VALVE_OFF, LOW);
  }else{
    digitalWrite(LED_VALVE_ON, LOW);
    digitalWrite(LED_VALVE_OFF, HIGH);
  }
    
  

}