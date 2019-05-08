#include <NewPing.h>
#include <Servo.h>
#include <Wire.h> //I2C Arduino Library

int HMC6352Address = 0x42;
int slaveAddress;
byte headingData[2];
int i, headingValue;

Servo myservo;
Servo angleservo;// create servo object to control a servo
Servo angleservo2;
int potpin = A0;  // analog pin used to connect the potentiometer
int val;

#define PING_PIN  12  // Arduino pin tied to both trigger and echo pins on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

void setup() {
  slaveAddress = HMC6352Address >> 1;   // This results in 0x21 as the address to pass to TWI
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  myservo.attach(5);
  angleservo.attach(6);// attaches the servo on pin 9 to the servo object

  //compass
  Wire.begin();
}
int pos = 0;
bool done = false;
void loop() {
  Serial.println(GetCompassData());
  if(!done){
    turnToMaxDistance();
    done = true;
  }
  delay(500);

}


float GetCompassData()
{
  Wire.beginTransmission(slaveAddress);
  Wire.write("A");              // The "Get Data" command
  Wire.endTransmission();

  Wire.requestFrom(slaveAddress, 2);        // Request the 2 byte heading (MSB comes first)
  i = 0;
  while (Wire.available() && i < 2)
  {
    headingData[i] = Wire.read();
    i++;
  }
  headingValue = headingData[0] * 256 + headingData[1]; // Put the MSB and LSB together
  return (headingValue / 10.0);     // The whole number part of the heading
}

void Direct()
{
  Serial.print("Ping: ");
  int val = sonar.ping_cm();
  Serial.print(val); // Send ping, get distance in cm and print result (0 = outside set distance range)

  Serial.println("cm");

  if (val > 40 || val == 0)
  {
    //Serial.println("Messze");
    SetServo(110, 90);
  }
  else if (val > 30)
  {
    //Serial.println("Közelebb");
    SetServo(100, 90);
  }
  else
  {
    Serial.println("Nagyon közel");
    SetServo(70, 90);
    delay(500);
    turn(90, 1);
  }

}

float findDegree(int goalDegree)
{
  Serial.println("Searching for Zero degree ");
  float degree = GetCompassData();
  if (((int)(GetCompassData()- goalDegree )% 360) < 10)
  {
    Serial.print("Insta win: ");
    return degree;  
  }
  SetServo(90, 120);
  delay(10);
  while(((int)(GetCompassData()- goalDegree )% 360) > 10){
    Serial.println("Searching...");
    Serial.print("act: ");
    Serial.println(GetCompassData());
    delay(100);
  }
  Serial.print("Found Zero degree: ");
  Serial.println(GetCompassData());
  SetServo(90, 90);
  delay(10000);
  return GetCompassData();
}

void turnToMaxDistance(){
  float startPoint = findDegree(0);
  Serial.println("Searching for Max Distance ");
  int distance = sonar.ping_cm();
  Serial.print("Default distance: ");
  Serial.println(distance);
  float degree = GetCompassData();
  Serial.print("Default degree: ");
  Serial.println(degree);
  SetServo(110, 90);
  delay(100);
  Serial.print("Default diff: ");
  Serial.println(((int)(startPoint- GetCompassData()) % 360));
  while( ((int) (startPoint- GetCompassData()) % 360) > 5.0 ){
    delay(10);
    int act = sonar.ping_cm();
    if(act > distance){
      distance = act;
      degree = GetCompassData();
      Serial.print("Found bigger distance: ");
      Serial.println(distance);
      Serial.print("Found bigger distance(degree): ");
      Serial.println(degree);
    }
    //SetServo(90, 120);
  }
  delay(10000);
  findDegree((int) degree);
  SetServo(90, 90);
}


void turn(int degree, int direction) {
  int act = (int)GetCompassData();
  float goal = (direction == 0) ? (float)((act + degree) % 360) : (float)((act - degree) % 360);

  Serial.print("Goal: ");
  Serial.println(goal);
  Serial.print("act: ");
  Serial.println(act);
  Serial.println("difference: ");
  Serial.print(abs(goal - GetCompassData()));
  Serial.println(abs(goal - GetCompassData()) < 20.0);


  while (abs(int(GetCompassData() - goal)) % 360 > 5.0)
  {
    Serial.println("ciklus");
    Serial.println(abs(int(GetCompassData() - goal) % 360));

    direction == 0 ? SetServo(90, 120) : SetServo(90, 60);
    delay(50);
  }
}

void StopServo()
{
  myservo.detach();
  angleservo.detach();
  
}
void StartServo()
{
  myservo.attach(5);
  angleservo.attach(6);
}
void SetServo(int speed, int angle)
{
  myservo.write(speed);
  angleservo.write(angle);
}
