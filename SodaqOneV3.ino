void setup() 
{
  SerialUSB.begin(9600);
  Serial.begin(9600);
  // Serial1.begin(9600);
}

void loop() 
{
  /*
  char arr[20] = "Hello world\n";

  for (int i=0; i<sizeof(arr); i++)
  {
    Serial.write(arr[i]);
  }
  */
  Serial.write('e');
  Serial.write('\n');

  while(Serial.available() == 0) {}

  if (Serial.available() > 0)
  {
    SerialUSB.write(Serial.read());
  }

  delay(500);
}
