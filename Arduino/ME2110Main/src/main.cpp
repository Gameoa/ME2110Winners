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

// Defining robot activation states using enum
enum ActivationState {
  ACT_OFF,
  ACT_ON
};

// Struct to manage activation input of robot
struct activation{
  ActivationState m_state = ACT_OFF;
  ActivationState m_prevState = m_state;
  float m_activationValue = 0;
  const float m_threshhold = .6;
  int m_port;

  activation(int port) {
    m_port = port;
  }

  void update() {
    m_prevState = m_state;
    int button = robot.readButton(m_port);
    //Biased activation sensing to prioritize reliable activation over reliable deactivation
    if(button){
      m_activationValue = 0.5 * float(button) + 0.5 * m_activationValue;
    }
    else{
      m_activationValue = 0.2 * float(button) + 0.8 * m_activationValue;
    }
    //Setting activation value based on set activation threshhold
    if (m_activationValue > m_threshhold){
      m_state = ACT_ON;
    }
    else {
      m_state = ACT_OFF;
    }
    /*Serial.print("Activation: ");
    Serial.print(m_activationValue);
    if(m_state != m_prevState){
      if(m_state == ACT_ON){
        Serial.print(" System Activated");
      }
      else{
        Serial.print(" System Deactivated");
      }
    }
    Serial.println();*/
  }

  ActivationState getState() {
    return m_state;
  }  

  int getStateInt() {
    if(m_state == ACT_OFF){
      return 0;
    }
    else if(m_state == ACT_ON){
      return 1;
    }
    return -1;
  }

  bool getEdgeDet() {
    if(m_state != m_prevState){
      return 1;
    }
    else {
      return 0;
    }
  }
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
int ultrasonicDist = robot.readUltrasonic();
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

  
  //update timeStart if compInput was just triggered on (resets activation loop)
  if (CompInput.getState() == ACT_ON && CompInput.getEdgeDet() == 1){
    timeStart = millis();
  }

  switch(currentState){
    previousState = currentState;
    case ROBOT_WAITING:
    //wait for comp input
    //if CompInput is true, start moving robot
    if(CompInput.getState()){
      currentState = ROBOT_MOVING;
    }
    break;
    case ROBOT_MOVING:
    //moving to center
    robot.moveMotor(1,1,255);
    if(limitSwitch.getValInt() == 1){
      robot.moveMotor(1,1,0);
      currentState = ROBOT_CENTER;
    }
    break;
    case ROBOT_CENTER:
    //performing center tasks
    int solenoid = 0;
    if(ultrasonicDist <= 25){
      if(millis() - lastFireTime > 1000){
        solenoid = 1;
        lastFireTime = millis();
      }
      else{
        solenoid = 0;
      }
    }
    else{
      solenoid = 0;
    }

    if(millis() - lastFireTime < 200){
      solenoid = 1;
    }

    switch(solenoid){
      case(0):
        lumaSolenoid.off();
        break;
      case(1):
        lumaSolenoid.on();
        break;
      default:
        lumaSolenoid.off();
        break;
    }
  }

  //backup mode switch to know at all times if robot is centered
  if(limitSwitch.getValInt() == 1){
    currentState = ROBOT_CENTER;
  }

  //update timeStart if compInput is off (prevents blocking of activation loop)
  if (CompInput.getState() == ACT_OFF){
    currentState = ROBOT_WAITING;
    timeStart = millis();
  }

  
  
  Serial.print(CompInput.getStateInt());
  Serial.print(" ");
  switch(currentState){
    case ROBOT_WAITING:
      Serial.print("0 ");
    break;
    case ROBOT_MOVING:
      Serial.print("1 ");
    break;
    case ROBOT_CENTER:
      Serial.print("2 ");
    break;
  }
  UINT32_MAX;
  Serial.print(millis());
  Serial.print(" ");
  Serial.print(analogRead(0));
  Serial.print(" ");
  Serial.print(limitSwitch.getValInt());
  Serial.print(" ");
  Serial.println(timeStart);
  
  //delay(20); // Short delay to avoid flooding the serial monitor
}
