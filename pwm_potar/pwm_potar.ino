int valPot = 0;


void setup()
{
  pinMode(3, OUTPUT);
  analogWrite(3, 0); 
}

void loop()
{
  valPot = analogRead(A0);
  valPot /= 4;
  analogWrite(3, valPot);
}
