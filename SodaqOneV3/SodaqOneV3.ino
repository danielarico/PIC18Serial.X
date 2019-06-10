#define interr A0
#define led A1

void setup() 
{
  SerialUSB.begin(9600);
  Serial.begin(9600);
  // Serial1.begin(9600);

  pinMode(interr, INPUT);
  pinMode(led, OUTPUT);

  digitalWrite(led, LOW);
}

void loop() 
{
  if (digitalRead(interr) == HIGH)
  {
    digitalWrite(led, HIGH);
    delay(500);
    Serial.write('e');
    Serial.write('\n');

    while(Serial.available() == 0) {}

    if (Serial.available() > 0)
    {
      SerialUSB.write(Serial.read());
    }
    
    digitalWrite(led, LOW);
    delay(500);
  }
  
  /*
  char arr[20] = "Hello world\n";

  for (int i=0; i<sizeof(arr); i++)
  {
    Serial.write(arr[i]);
  }
  */
  
}
