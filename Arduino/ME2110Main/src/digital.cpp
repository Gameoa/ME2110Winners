#include <digital.h>

// Defining digital output states using enum
enum DigState {
  DIG_OFF,
  DIG_ON,
  DIG_DEFAULT
};
struct digInput {
  DigState m_state = DIG_OFF;
  DigState m_prevState = m_state;
  int m_port;
  DigState m_def;

  digInput(int port) {
    m_port = port;
    update();
  }

  void update(){
    int input = robot.readButton(m_port);
    m_prevState = m_state;
    switch (input){
      case 0:
      m_state = DIG_OFF;
        break;
      case 1:
      m_state = DIG_ON;
        break;
      default:
      m_state = DIG_OFF;
        break;
    }
  }

  int getValInt(){
    switch(m_state){
      case DIG_ON:
      return 1;
        break;
      case DIG_OFF:
      return 0;
        break;
      default:
      return 0;
        break;
    }
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

// Struct to manage digital outputs
struct digOutput {
  DigState state = DIG_DEFAULT;
  int m_port;
  DigState m_def;

  digOutput(int port, DigState def) {
    m_port = port;
    m_def = def;
  }

  DigState on() {
    DigState prev = state;
    state = DIG_ON;
    robot.digital(m_port, 1);
    return prev;
  }

  DigState off() {
    DigState prev = state;
    state = DIG_OFF;
    robot.digital(m_port, 0);
    return prev;
  }

  DigState read() {
    return state;
  }

  void reset() {
    state = DIG_DEFAULT;
    robot.digital(m_port, m_def);
  }
};