

int sensorpin=A2;
int dire=A5;
int sensor2=A4;
int dire2=A3;
volatile unsigned long my_array[3];
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(sensorpin,INPUT);
pinMode(dire,INPUT);
pinMode(sensor2,INPUT);
pinMode(dire2,INPUT);
}
void loop() 
{
 int sensorval=analogRead(sensorpin);
 int voltageval=(sensorval/1024.0)*5.0;//converts analog values into either 1 or 0.
  //Serial.print("vval:");
 //Serial.print(voltageval);
 int direval=analogRead(dire);
 int dirval=(direval/1024.0)*5.0;
 int sensorval2=analogRead(sensor2);
 int voltageval2=(sensorval2/1024.0)*5.0;//converts analog values into either 1 or 0.
  //Serial.print("vval:");
 //Serial.print(voltageval);
 int direval2=analogRead(dire2);
 int dirval2=(direval2/1024.0)*5.0;
 //Serial.print("\ndirval:");
 //Serial.print(dirval);
 if(voltageval<2&&dirval==2)
 {
  unsigned long rad1=millis();
  rad1=my_array[1];
  Serial.print("arra1");
  Serial.print(rad1);
 }
  if(voltageval2<2&&dirval2==2)
 {
  unsigned long rad2=millis();
  rad2=my_array[2];
  Serial.print("arra2");
  Serial.print(rad2);
 }
 if(my_array[1]>my_array[2])
 {
Serial.print("\nright");
delay(600);
}
else if(my_array[1]<my_array[2])
{
 Serial.print("\nleft");
 delay(600);
}
else{
  Serial.print("\nstandby");
  delay(100);
  //Serial.print("arra1");
  //Serial.print(my_array[1]);
  /*Serial.print("arra2");
  Serial.print(my_array[2]);*/
}
}
