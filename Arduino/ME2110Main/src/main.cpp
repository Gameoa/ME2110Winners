#include <myDuino.h>
#include <digital.h>
#include <activation.h>
#include <lift.h>
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

long int liftTime = 9300;
long int liftHoldTime = 5000;
//int liftTime = 0;
long int driveTime = 7000;

bool extensionFired = false;

int extensionFireDelay = 0;
int extensionRetractDelay = 1000;

bool lumaTriggered = false;
long int lumaFireDelay = 1000                                                              ;
long int currentLumaFireDelay = lumaFireDelay;
long int lumaRetriggerDelay = 1000;
long int currentLumaRetriggerDelay = 0;
long int lumaRetractDelay = 50;
long int currentLumaRetractDelay = lumaRetractDelay;
int lumaFireNum = 4;

long int koopaDelay = 35000;
int koopaFireTime = 100;
int currentKoopaFireTime = koopaFireTime;
int koopaRestartTime = 500;
int currentKoopaRestartTime = koopaRestartTime;

int irCalibrationTime = 5000;

digOutput koopaSolenoid(1, DIG_OFF);
digOutput lumaPiston(2, DIG_OFF);
digOutput extensionPiston(3, DIG_OFF);

int liftSwitchPort = 3;

digInput lumaSwitch(2);
digInput liftSwitch(liftSwitchPort);

//normal code
Lift marioLift(LIFT_MOTOR, false, 300, 9300, liftSwitchPort);
//retract code
//Lift marioLift(LIFT_MOTOR, false, -10000, 0, liftSwitchPort);

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
  lumaSwitch.update();
  liftSwitch.update();
  marioLift.update(millis());
  //limitSwitch.update();
  //pot = robot.readPOT();
  //robot.doEncoder();
  //encoder = robot.encoderCount();
  //encoderDir = robot.encoderDir();
  //ultrasonicDist = robot.readUltrasonic();

  //Sets default motor state to off at the beginning of each loop to avoid unintended motor movement due to state changes
  //DO NOT COMMENT OUT
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

  /* //lift return code
  if(CompInput.getState() == ACT_ON){
    marioLift.moveDown();
  }
  else {
    marioLift.stop();
  }
  //uncomment to allow for testing
  currentState = ROBOT_WAITING; */

  switch(currentState) {
    case ROBOT_WAITING:
      marioLift.stop();
      
      if(CompInput.getState() == ACT_ON){
        if(extensionFireDelay > 0){
          extensionFireDelay -= timeDelta;
        }
        else{
          if(extensionRetractDelay > 0){
            extensionPiston.setState(DIG_ON);
            extensionFired = true;
            extensionRetractDelay -= timeDelta;
          }
          else{
            extensionPiston.setState(DIG_OFF);
            currentState = ROBOT_MOVING;
          }
        }
      }
      break;
    case ROBOT_MOVING:
      if(driveTime > 0){
        robot.moveMotor(DRIVE_MOTOR, 1, 255);
        marioLift.stop();
        driveTime -= timeDelta;
      }
      else{
        currentState = ROBOT_CENTER;
        timeAtCenter = millis();

      }
      break;
    case ROBOT_CENTER:
      robot.moveMotor(DRIVE_MOTOR, 1, 127);
      if(liftTime > 0){
        //robot.moveMotor(LIFT_MOTOR, 1, 255);
        marioLift.moveUp();
        liftTime -= timeDelta;
      }
      else if(liftHoldTime > 0){
        liftHoldTime -= timeDelta;
        marioLift.stop();
      }
      else{
        marioLift.moveDown();
      }

      if(lumaSwitch.getValInt() == 1){
        if(currentLumaRetriggerDelay <= 0){
          lumaTriggered = true;
          currentLumaRetriggerDelay = lumaRetriggerDelay;
        }
      }

      if(currentLumaRetriggerDelay > 0){
        currentLumaRetriggerDelay -= timeDelta;
      }

      if(lumaTriggered == true){
        lumaTriggered = true;
        if(currentLumaFireDelay > 0){
          currentLumaFireDelay -= timeDelta;
        }
        else{
          if(currentLumaRetractDelay > 0){
            lumaPiston.setState(DIG_ON);
            //lumaTriggered = true;
            currentLumaRetractDelay -= timeDelta;
          }
          else{
            lumaTriggered = false;
            lumaPiston.setState(DIG_OFF);
            currentLumaFireDelay = lumaFireDelay;
            currentLumaRetractDelay = lumaRetractDelay;
            lumaFireNum = lumaFireNum - 1;
          }
        }
      }

      /* if(lumaSwitch.getValInt() == 1 or lumaTriggered == true){
        lumaTriggered = true;
        if(currentLumaFireDelay > 0){
          currentLumaFireDelay -= timeDelta;
        }
        else{
          if(currentLumaRetractDelay > 0){
            lumaPiston.setState(DIG_ON);
            //lumaTriggered = true;
            currentLumaRetractDelay -= timeDelta;
          }
          else{
            lumaTriggered = false;
            lumaPiston.setState(DIG_OFF);
            currentLumaFireDelay = lumaFireDelay;
            currentLumaRetractDelay = lumaRetractDelay;
            lumaFireNum = lumaFireNum - 1;
          }
        }
      } */
      /* if(irCalibrated == false){
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
      } */

      /* if(CompInput.getState() == ACT_OFF){
        koopaDelay = 37000;
      } */

      if(koopaDelay > 0){
        koopaDelay -= timeDelta;
      }
      else{
        if(currentKoopaFireTime > 0){
          koopaSolenoid.setState(DIG_ON);
          currentKoopaFireTime -= timeDelta;
        }
        else{
          if(currentKoopaRestartTime > 0){
            koopaSolenoid.setState(DIG_OFF);
            currentKoopaRestartTime -= timeDelta;
          }
          else{
            currentKoopaFireTime = koopaFireTime;
            currentKoopaRestartTime = koopaRestartTime;
          }
        }
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
  switch(currentState){
    case ROBOT_WAITING:
      Serial.print("1");
      break;
    case ROBOT_MOVING:
      Serial.print("2");
      break;
    case ROBOT_CENTER:
      Serial.print("3");
      break;
  }
  Serial.print(" ");
  Serial.print(timeNow);
  Serial.print(" ");
  Serial.print(timeDelta);
  Serial.print(" ");
  Serial.print(extensionFireDelay);
  Serial.print(" ");
  Serial.print(extensionRetractDelay);
  Serial.print(" ");
  Serial.print(driveTime);
  Serial.print(" ");
  Serial.print(marioLift.getPosition());
  Serial.print(" ");
  Serial.print(liftSwitch.getValInt());
  Serial.print(" ");
  Serial.print(marioLift.atBottomLimit());
  Serial.print(" ");
  Serial.print(lumaTriggered);
  Serial.print(" ");
  Serial.print(currentLumaFireDelay);
  Serial.print(" ");
  Serial.print(currentLumaRetractDelay);
  Serial.print(" ");
  Serial.print(koopaDelay);
  //Serial.print(irCalibrated);
  //Serial.print(" ");
  //Serial.print(irDist);
  //Serial.print(" ");
  //Serial.print(irmax);
  //Serial.print(" ");
  //Serial.print(irmin);
  Serial.println();
  
  delay(20); // Short delay to avoid flooding the serial monitor
}
