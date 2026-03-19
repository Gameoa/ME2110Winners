#include <myDuino.h>
#include <digital.h>
#include <activation.h>
#include <Arduino.h>

myDuino robot(1);

// Defining robot process state using enum
enum RobotState {
  ROBOT_WAITING,
  ROBOT_MOVING,
  ROBOT_CENTER
};

//handle for attaching pin interrupt
//void encHandler(){
//  robot.doEncoder();
//}

//Setting Program Variables and classes

RobotState currentState = ROBOT_WAITING;
RobotState previousState = ROBOT_WAITING;
//int pot;
//int encoder;
//int encoderDir;
long unsigned int timeStart  = 0;
//int ultrasonicDist = robot.readUltrasonic();
long unsigned int lastFireTime = 0;
digOutput lumaSolenoid(1, DIG_OFF);
//digOutput sol2(2, DIG_OFF);
//digOutput act1(3, DIG_OFF);
//digOutput act2(4, DIG_OFF);
digInput limitSwitch(4);


activation CompInput(1);

void setup() {
  //attachInterrupt(digitalPinToInterrupt(2), encHandler, CHANGE); // attach encoder channel 1
  //attachInterrupt(digitalPinToInterrupt(3), encHandler, CHANGE); // attach encoder channel 2
  Serial.begin(115200);
  Serial.println("Robot State Machine Test");
}

void loop() {
  CompInput.update();
  limitSwitch.update();
  //pot = robot.readPOT();
  //robot.doEncoder();
  //encoder = robot.encoderCount();
  //encoderDir = robot.encoderDir();
  //ultrasonicDist = robot.readUltrasonic();


  Serial.print(CompInput.getStateInt());
  Serial.print(" ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.print(analogRead(0));
  
  delay(20); // Short delay to avoid flooding the serial monitor
}
