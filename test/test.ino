


///////////////////////FUEL LEVEL INDICATOR////////////////////////////////////////////////////////////
const int fuel_sensor = A11;

const int A = 33;
const int B = 35;
const int C = 37;
const int D = 39;

///////////////////////RAIN AND SERVO///////////////////////////////////////////////////////////
int rainPin = A13;
// threshold value( adjust it if it's too low
int thresholdValue = 500;

#include <Servo.h>
#include <TEA5767N.h>
#include <Wire.h>
TEA5767N Radio = TEA5767N(); // Using standard I2C pins A4 and A5


Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

/////////////////////////////////RADIO/////////////////////////////////////////////////////////
float stations[] = {87.8, 89.5, 95.4};
int index = 0;

int inByte;
int flag=0;

int x = 0;
bool received = false;

bool on = false;



/////////////////////////////////////H-BRIDGE MODULE////////////////////////////////////////////

/*
 * motor direction control.
 * Motor A: if in1 is set => forward. (with respect to our car)
 * Motor B: if in3 is set => forward. (with respect to our car)
 */

 int motorA_in1 = 3;
 int motorA_in2 = 4;
 int motorB_in3 = 5;
 int motorB_in4 = 6;



 /*
  * motors enable (if we supply digital input) / speed control (if analog input by PMW).
  */
int motorA_speed = 2;
int motorB_speed = 7;
int val = 80;

/*
 * COMMUNICATION
 * -Bluetooth Module
 */
 
/////////////////////////////////////BLUETOOTH MODULE////////////////////////////////////////////
char bluetooth; //use to store the data read into the MCU.


////////////////////////////////////DETECTION MODULE/////////////////////////////////////////////
int U1Trigger = 31;
int U1Input = 30;
int buzzer = 8;

void setup()
{
 
  Serial.begin(9600);
   pinMode(U1Trigger, OUTPUT);
   pinMode(U1Input, INPUT);
 myservo.attach(12);
   pinMode(fuel_sensor, INPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);

  // put your setup code here, to run once:
  //i2c connection on 9
  Wire.begin(9);        
  Serial.begin(9600);
  //Radio.init();
  Radio.mute();
  Radio.selectFrequency(stations[0]); //On power on go to first station
  //Serial.println(stations[0]);

  
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  received = true;
}

void receiveScreen() {
  if(received)
  {
    received = false;
    Serial.println(x);
    //power button is pressed
    if(x==0)
    {
      Serial.println("here");
      if(on)
      {
        Radio.mute();
      }
      else
      {
        Radio.turnTheSoundBackOn();
        Serial.println(stations[index]);
      }
  
      on = !on;
    }
    //prev is pressed
    else if(x==1)
    {
      index--;
      if(index==-1)
      {
        index = 2;
      }
      Serial.println(index);
      Radio.selectFrequency(stations[index]);
      Serial.println(stations[index]);
    }
    //next is pressed
    else if(x==2)
    {
      index++;
      if(index==3)
      {
        index = 0;
      }
      Radio.selectFrequency(stations[index]);
      Serial.println(stations[index]);
    }
    delay(500);
    Serial.println("finished delaying");
  }
}

void turnRight(){
  digitalWrite(motorA_in1,HIGH); 
  digitalWrite(motorA_in2,LOW); 
  digitalWrite(motorB_in3,HIGH); 
  digitalWrite(motorB_in4,LOW); 
  analogWrite(motorA_speed, 120);
  analogWrite(motorB_speed, val/5);
  //delay(600);
  //goFront();
}

void turnLeft(){
  digitalWrite(motorA_in1,HIGH); 
  digitalWrite(motorA_in2,LOW); 
  digitalWrite(motorB_in3,HIGH); 
  digitalWrite(motorB_in4,LOW); 
  analogWrite(motorA_speed, val/5);
  analogWrite(motorB_speed, 120);
  //delay(600);
  //goFront();
}

void goFront(){
  digitalWrite(motorA_in1,HIGH); 
  digitalWrite(motorA_in2,LOW); 
  digitalWrite(motorB_in3,HIGH); 
  digitalWrite(motorB_in4,LOW); 
  analogWrite(motorA_speed, val);
  analogWrite(motorB_speed, val);
}

void goBack(){
  digitalWrite(motorA_in1,LOW); 
  digitalWrite(motorA_in2,HIGH); 
  digitalWrite(motorB_in3,LOW); 
  digitalWrite(motorB_in4,HIGH); 
  analogWrite(motorA_speed, val);
  analogWrite(motorB_speed, val);
}


double reportDistance(){
  //variables to store the recorded distance
   long duration, inches, cm;

  //triggering the sensor to receive an echo
   digitalWrite(U1Trigger, LOW);
   delay(2);
   digitalWrite(U1Trigger, HIGH);
   delay(10);
   digitalWrite(U1Trigger, LOW);

   //reading the input
   duration = pulseIn(U1Input, HIGH);
   cm = duration / 29 / 2;
   
   //displaying distance
   //Serial.print(cm);
   //Serial.print("cm");
   //Serial.println();
   delay(100);
   
   return cm;
}


int safeDistance(){
   long distance = reportDistance();
   if (distance >= 19 && distance < 35){
    analogWrite(buzzer, 85);  
    return 0; 
   }

   else if (distance >= 6 && distance < 19){
    analogWrite(buzzer, 85*2 + 60);
    return 0;
   }

   else if (distance<6){
    analogWrite(buzzer, 85*3);
    return 0;
   }

   else{
    analogWrite(buzzer, 0);
   }

   return 1;
}

void stopCar(int t){
  digitalWrite(motorA_in1,LOW); digitalWrite(motorA_in2,LOW); digitalWrite(motorB_in3,LOW); digitalWrite(motorB_in4,LOW); 
  delay(t);
}

int startParking(){
  turnRight();
  delay(720);

  goBack();
  int flag = 1;
  while (flag){
    flag = safeDistance();  
  }

  stopCar(300);
  turnLeft();
  delay(550);

  stopCar(100);
  
  return 1; //parked successfully, continue other stuff
  
}

void loop()
{
  const int minVal = 620;
  const int midVal = 680;

  int fuel_reading = analogRead(fuel_sensor);
  int fuel_level =  0;

  if (fuel_reading < minVal) {
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);

  }
  if (fuel_reading >= minVal && fuel_reading < midVal) {
    fuel_level = 1;
    digitalWrite(A, HIGH);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
  }
  if (fuel_reading >= midVal) {
    fuel_level = 2;
    digitalWrite(A, LOW);
    digitalWrite(B, HIGH);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
  }
  Serial.println(fuel_reading);

  int sensorValue = analogRead(rainPin);
  if(sensorValue < thresholdValue){
   // Serial.println(" - It's wet");

    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  }
  else {
   // Serial.println(" - It's dry");
    //digitalWrite(redLED, LOW);
  }


   //set up direction according to user's latest choice.
   if(Serial.available()>0){
    bluetooth = Serial.read();
    
    switch(bluetooth){ 
      case 'f': goFront(); break;
      case 'b': goBack();break;
      case 'r': turnRight(); break;
      case 'l': turnLeft(); break;
      case 's': stopCar(0);break;
      case 'p': startParking();break;
    }


    //////////////////////////////////////////////RADIO COD/////////////////////////////////////////////
     // put your main code here, to run repeatedly:
  receiveScreen();
  /*
  if (Serial.available()>0) {
    inByte = Serial.read();
    if (inByte == '+' || inByte == '-' || inByte == '*'){  //accept only + and - from keyboard
      flag=0;
    }
  }

  //If forward button is pressed, go up to next station
  if (inByte == '+') {
    index++;
    if(index==3)
    {
      index = 0;
    }
     Radio.selectFrequency(stations[index]);
    Serial.println(stations[index]);
  }
  //If backward button is pressed, go down to next station
  if (inByte == '-') {
    index--;
    if(index==-1)
    {
      index = 2;
    }
     Radio.selectFrequency(stations[index]);
    Serial.println(stations[index]);
  }

  if (inByte == '*') {
    if(on)
    {
      Radio.mute();
    }
    else
    {
      Radio.turnTheSoundBackOn();
    }
  }
  
  delay(500);
  */
    
  }
  
  
  safeDistance();
}
