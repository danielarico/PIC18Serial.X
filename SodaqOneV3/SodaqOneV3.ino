#define interr A0
#define led A1

const int arr_size = 10;
char str_rdy [arr_size] = "RDY\n";


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
  int flag = 0;
  
  if (digitalRead(interr) == HIGH)
  {
    digitalWrite(led, HIGH);
    delay(500);

    Serial.print(str_rdy);

    while(Serial.available() == 0) {}
    
    while(Serial.available() && flag == 0)
    {
      char a = Serial.read();
      SerialUSB.write(a);

      if (a == '\n')
      {
        flag = 1;
      }
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
