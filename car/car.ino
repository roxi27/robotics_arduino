#include <NewPing.h>
#include <Servo.h>
#include <Wire.h>

int HMC6352Address = 0x42;
int slaveAddress;
byte headingData[2];
int i, headingValue;

Servo myservo;
Servo angleservo;
Servo angleservo2;
int potpin = A0;
int val;

#define PING_PIN  12
#define MAX_DISTANCE 200

NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE);

void setup() {
  slaveAddress = HMC6352Address >> 1;
  Serial.begin(115200);
  myservo.attach(5);
  angleservo.attach(6);

  Wire.begin();
}

void loop() {

  Direct();
  delay(1000);
}

void Direct()
{
  Serial.print("Ping: ");
  int val = sonar.ping_cm();
  Serial.print(val);

  Serial.println("cm");

  if (val > 50 || val == 0)
  {
    SetServo(110, 90);
  }
  else if (val > 30)
  {
    SetServo(100, 90);
  }
  else
  {
    SetServo(70, 90);
    delay(500);

    turnToMaxDistance();
  }

}

float GetCompassData()
{
  Wire.beginTransmission(slaveAddress);
  Wire.write("A");
  Wire.endTransmission();

  Wire.requestFrom(slaveAddress, 2);
  i = 0;
  while (Wire.available() && i < 2)
  {
    headingData[i] = Wire.read();
    i++;
  }
  headingValue = headingData[0] * 256 + headingData[1];
  return (headingValue / 10.0);
}


void turnToMaxDistance() {
  float startPoint = GetCompassData();
  Serial.print("Startpoint:");
  Serial.println(startPoint);

  Serial.println("Searching for Max Distance ");
  int distance = sonar.ping_cm();

  Serial.print("Default distance: ");
  Serial.println(distance);
  float degree = GetCompassData();

  Serial.print("Default degree: ");
  Serial.println(degree);

  SetServo(90, 120);

  while (isNear(startPoint, GetCompassData(), 15))
  {
    delay(5);
  }
  Serial.print("Start degree: ");
  Serial.println(GetCompassData());

  Serial.print("Default diff: ");
  Serial.println(getDegree(GetCompassData() - startPoint));

  while (!isNear(startPoint, GetCompassData(), 5)) {
    delay(5);

    int act = sonar.ping_cm();
    if (act > distance) {
      distance = act;
      degree = GetCompassData();

      Serial.print("Found bigger distance: ");
      Serial.println(distance);

      Serial.print("Found bigger distance(degree): ");
      Serial.println(degree);
    }
  }

  SetServo(90, 90);
  delay(300);
  findDegree((int) degree);
  SetServo(90, 90);
}


float findDegree(int goalDegree)
{
  Serial.print("Searching for goal degree: ");
  Serial.println(goalDegree);

  float degree = GetCompassData();
  Serial.print("Start degree for search: ");
  Serial.println(degree);

  if (isNear(goalDegree, GetCompassData(), 5))
  {
    Serial.print("Insta win: ");
    Serial.println(getDegree(GetCompassData() - goalDegree));

    return degree;
  }

  turn(goalDegree);

  Serial.print("Found goal degree: ");
  Serial.println(GetCompassData());

  SetServo(90, 90);
  delay(500);

  return GetCompassData();
}

int getShorterDirection(double goal) {
  return getDegree(goal - GetCompassData()) < 180.1 ? 1 : 0;
}

int getDegree(double value) {
  if (value >= 0.0 && value < 360.0) {
    return (int) value;
  }

  if (value < 0.0) {
    return getDegree(value + 360.0);
  }

  if (value >= 360.0) {
    return getDegree(value - 360.0);
  }
}

bool isNear(double goal, double actual, double threshold) {

  return (getDegree(actual) >= (int)(goal - threshold) && getDegree(actual) <= (int)(goal + threshold));
}


void turn(double goal) {
  getShorterDirection(goal) ? SetServo(90, 120) : SetServo(90, 60);
  while (!isNear(goal, GetCompassData(), 5))
  {
    delay(5);
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
