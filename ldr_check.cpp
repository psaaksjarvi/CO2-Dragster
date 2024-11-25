static const uint8_t analog_pins[] = {A0,A1,A2,A3,A4, A5};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0, INPUT);

  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
// Serial.println(analogRead(A0));
//   Serial.println(analogRead(A1));
// Serial.println(analogRead(A2));
  Serial.println(analogRead(A3));
Serial.println(analogRead(A4));
Serial.println(analogRead(A5));
// for (int i = 0; i < 6; i++)
// {
//   Serial.print(i);
//   Serial.print(": ");
//   Serial.println(analogRead(A0 + i));
//   // Serial.println(analogRead(analog_pins[i]));
  delay(1000);
// }



// delay(200);
}
