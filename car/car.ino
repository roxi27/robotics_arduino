#include <NewPing.h>
#include <Servo.h>

Servo myservo; 
Servo angleservo;// create servo object to control a servo

int potpin = A0;  // analog pin used to connect the potentiometer
int val;    

#define PING_PIN  12  // Arduino pin tied to both trigger and echo pins on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  myservo.attach(5);  
  angleservo.attach(6);// attaches the servo on pin 9 to the servo object
}
int pos = 0;
void loop() {
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Ping: ");
  int val = sonar.ping_cm();
  Serial.print(val); // Send ping, get distance in cm and print result (0 = outside set distance range)
 
  Serial.println("cm");

  if(val > 40)
  {
    SetServo(130,90);
  }
  else if(val > 30)
  {
     SetServo(110,90);
  }
  else
  {
    SetServo(50, 90);
    delay(500);
    SetServo(90, 150);
    delay(500);
  }
 
}

void SetServo(int speed, int angle)
{
   myservo.write(speed);  
   angleservo.write(angle);
}
