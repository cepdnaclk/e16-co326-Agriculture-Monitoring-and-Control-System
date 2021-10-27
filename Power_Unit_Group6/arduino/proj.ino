int analogPin = A0; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V

int analogPin2 = A1;

int val1 = 0;
int val2 = 0;// variable to store the value read

void setup() {
  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);

//  Serial.begin(9600);           //  setup serial
}

void loop() {
  val1 = analogRead(analogPin);  // read the input pin
  val2 = analogRead(analogPin2);
  
  if(val1 > 220){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }
  
  else 
  {
    if(val2>4)
    {
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
    }
    if(val2<2)
    {
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
    }
  }
//  Serial.println(val);          // debug value
}
