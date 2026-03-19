#pragma once
#include <myDuino.h>

extern myDuino robot;

// Defining robot activation states using enum
enum ActivationState {
  ACT_OFF,
  ACT_ON
};

// Struct to manage activation input of robot
struct activation{
  ActivationState m_state = ActivationState::ACT_OFF;
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
      m_state = ActivationState::ACT_ON;
    }
    else {
      m_state = ActivationState::ACT_OFF;
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
    if(m_state == ActivationState::ACT_OFF){
      return 0;
    }
    else if(m_state == ActivationState::ACT_ON){
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