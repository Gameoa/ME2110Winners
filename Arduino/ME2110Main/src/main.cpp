#include <myDuino.h>
#include <digital.h>
#include <activation.h>
#include <Arduino.h>

myDuino robot(1);

#define DRIVE_MOTOR 1
#define LIFT_MOTOR 2

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
long unsigned int timeNow = 0;
long unsigned int timePrevious = 0;
unsigned int timeDelta = 0;
//int ultrasonicDist = robot.readUltrasonic();

long unsigned int lastFireTime = 0;

unsigned long int timeAtCenter = 0;
//unsigned long int timeAtMoving = 0;

int liftTime = 11000;
int driveTime = 6300;

int lumaFireNum = 1;

int irCalibrationTime = 5000;

int koopaDelay = 10000;
digOutput koopaSolenoid(1, DIG_OFF);
digOutput lumaPiston(2, DIG_OFF);
//digOutput act1(3, DIG_OFF);
//digOutput act2(4, DIG_OFF);
//digInput limitSwitch(4);


activation CompInput(1);

void setup() {
  //use only for ultrasonic sensor
  //attachInterrupt(digitalPinToInterrupt(2), encHandler, CHANGE); // attach encoder channel 1
  //attachInterrupt(digitalPinToInterrupt(3), encHandler, CHANGE); // attach encoder channel 2
  Serial.begin(115200);
  Serial.println("Robot State Machine Test");
}

int irPrevDist = 0;
int irDist = 0;

void loop() {
  CompInput.update();
  //limitSwitch.update();
  //pot = robot.readPOT();
  //robot.doEncoder();
  //encoder = robot.encoderCount();
  //encoderDir = robot.encoderDir();
  //ultrasonicDist = robot.readUltrasonic();

  robot.moveMotor(DRIVE_MOTOR, 1, 0);
  robot.moveMotor(LIFT_MOTOR, 1, 0);

  irPrevDist = irDist;
  irDist = robot.readIR();
  int irmin = 0;
  int irmax = 0;
  int irMinThreshold = 0;
  int irMaxThreshold = 0;
  bool irCalibrated = false;

  if(CompInput.getState() == ACT_OFF){
    currentState = ROBOT_WAITING;
  }

  switch(currentState) {
    case ROBOT_WAITING:
      robot.moveMotor(DRIVE_MOTOR, 1, 0);
      robot.moveMotor(LIFT_MOTOR, 1, 0);
      if(CompInput.getState() == ACT_ON){
        currentState = ROBOT_MOVING;
        timeStart = millis();
      }
      break;
    case ROBOT_MOVING:
      if(driveTime > 0){
        robot.moveMotor(DRIVE_MOTOR, 1, 255);
        robot.moveMotor(LIFT_MOTOR, 1, 0);
        driveTime -= timeDelta;
      }
      else{
        currentState = ROBOT_CENTER;
        timeAtCenter = millis();

      }
      break;
    case ROBOT_CENTER:
      robot.moveMotor(DRIVE_MOTOR, 1, 10);
      if(liftTime > 0){
        //robot.moveMotor(LIFT_MOTOR, 1, 255);
        liftTime -= timeDelta;
      }
      else{
        //robot.moveMotor(LIFT_MOTOR, 1, 0);
      }

      if(irCalibrated == false){
        if(irDist > irmax){
          irmax = irDist;
        }
        if(irDist < irmin || irmin == 0){
          irmin = irDist;
        }
        if(irCalibrationTime > 0){
          irCalibrationTime -= timeDelta;
        }
        else{
          irCalibrated = true;
          irMaxThreshold = irmin + (irmax - irmin) * 0.9;
          irMinThreshold = irmin - (irmax - irmin) * 0.5;
        }
      }
      else{
        if(lumaFireNum > 0){
          if(irDist > irMaxThreshold && irDist > irPrevDist){
            //object detected, fire solenoid
            lumaPiston.setState(DIG_ON);
            lumaFireNum = lumaFireNum - 1;
          }
          else if(irDist < irMinThreshold){
            lumaPiston.setState(DIG_OFF);
          }
        }
      }

      if(koopaDelay > 0){
        koopaDelay -= timeDelta;
      }
      else{
        koopaSolenoid.setState(DIG_ON);
      }
      break;
  }
    
  timePrevious = timeNow;
  timeNow = millis();
  timeDelta = timeNow - timePrevious;

  Serial.print(CompInput.getStateInt());
  Serial.print(" ");
  Serial.print(analogRead(0));
  Serial.print(" ");
  switch(CompInput.getState()){
    case ACT_OFF:
      Serial.print("0");
      break;
    case ACT_ON:
      Serial.print("1");
      break;
  }
  Serial.print(" ");
  Serial.print(timeNow);
  Serial.print(" ");
  Serial.print(timeDelta);
  Serial.print(" ");
  Serial.print(driveTime);
  Serial.print(" ");
  Serial.print(liftTime);
  Serial.print(" ");
  Serial.print(irCalibrated);
  Serial.print(" ");
  Serial.print(irDist);
  Serial.print(" ");
  Serial.print(irmax);
  Serial.print(" ");
  Serial.print(irmin);
  Serial.println();
  
  delay(20); // Short delay to avoid flooding the serial monitor
}
