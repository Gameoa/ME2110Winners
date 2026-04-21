#pragma once

#include <lift.h>
#include <myDuino.h>

extern myDuino robot;

class Lift {
public:
    Lift(int motorPort, bool reverse = false, int travelTimeStart = 1000, int travelTimeEnd = 1000, int limitPort = 0);

    void setLimitPort(int limitPort);

    void begin();
    void update(unsigned long now = millis());

    void moveUp();
    void moveDown();
    void moveTargetUp(int target);
    void moveTargetDown(int target);
    void stop();

    long int getPosition() const;
    bool atTop() const;
    bool atBottom() const;
    bool atBottomLimit() const;
    void setTravelTime(long int travelTimeStart, long int travelTimeEnd);
    void setTravelTimeStart(long int travelTimeStart);
    void setTravelTimeEnd(long int travelTimeEnd);

private:
    int _port = 0; // default port, can be set via constructor or setter if needed
    int _limitPort = 0; // optional limit switch port, not used in current implementation
    bool _reverse = false;
    long int _travelTimeStart; // default travel time in ms
    long int _travelTimeEnd;   // default travel time in ms
    unsigned long _lastUpdateMs;
    long int _position;      // _travelTimeStart <= _position <= _travelTimeEnd
    int8_t _direction;    // +1 = up, -1 = down, 0 = stopped

    void clampPosition();
};

Lift::Lift(int motorPort, bool reverse, int travelTimeStart, int travelTimeEnd, int limitPort)
    :   _port(motorPort),
        _reverse(reverse),
        _travelTimeStart(travelTimeStart),
        _travelTimeEnd(travelTimeEnd),
        _lastUpdateMs(0),
        _position(0.0f),
        _direction(0),
        _limitPort(limitPort) {}

void Lift::setLimitPort(int limitPort) {
    _limitPort = limitPort;
}

void Lift::begin() {
    _lastUpdateMs = millis();
    stop();
}

void Lift::update(unsigned long now) {
    unsigned long delta = now - _lastUpdateMs;
    _lastUpdateMs = now;

    if (_direction == 0 || _travelTimeEnd == _travelTimeStart) {
        return;
    }

    if (_direction > 0) {
        _position += delta;
    }

    if (_direction < 0) {
        _position -= delta;
    }

    if (atTop() || atBottom() || atBottomLimit()) {
        stop();
    }
}

void Lift::moveUp() {
    if (atTop()) {
        stop();
    } else {
        _direction = +1;
        if(_reverse) {
            robot.moveMotor(_port, 2, 255); // 2 = raise (reversed)
        } else {
            robot.moveMotor(_port, 1, 255); // 1 = raise
        }
    }
}

void Lift::moveDown() {
    if (atBottom() || atBottomLimit()) {
        stop();
    } else {
        _direction = -1;
        if(_reverse) {
            robot.moveMotor(_port, 1, 255); // 1 = lower (reversed)
        } else {
            robot.moveMotor(_port, 2, 255); // 2 = lower
        }
    }
}

void Lift::moveTargetUp(int target) {
    if (_position < target) {
        moveUp();
    }
    else {
        stop();
    }
}

void Lift::moveTargetDown(int target) {
    if(_position > target) {
        moveDown();
    }
    else {
        stop();
    }
}

void Lift::stop() {
    _direction = 0;
    robot.moveMotor(_port, 1, 0); // 0 speed = stop
}

long int Lift::getPosition() const {
    return _position;
}

bool Lift::atTop() const {
    return _position >= _travelTimeEnd;
}

bool Lift::atBottom() const {
    return _position <= _travelTimeStart;
}

bool Lift::atBottomLimit() const {
    //return robot.readButton(_limitPort); // assuming limit switch is active HIGH
    if (_limitPort == 0) {
        return atBottom();
    } else {
        return robot.readButton(_limitPort); // assuming limit switch is active HIGH
    }
}

void Lift::setTravelTime(long int travelTimeStart, long int travelTimeEnd) {
    _travelTimeStart = travelTimeStart;
    _travelTimeEnd = travelTimeEnd;
}

void Lift::setTravelTimeStart(long int travelTimeStart) {
    _travelTimeStart = travelTimeStart;
}

void Lift::setTravelTimeEnd(long int travelTimeEnd) {
    _travelTimeEnd = travelTimeEnd;
}