#ifndef SERVOHANDER_H
#define SERVOHANDLER_H

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "wiringPiI2C.h"

class ServoHandler {
public:
	ServoHandler();
	ServoHandler(int headerNum, int fd, int minVal, int maxVal);
	int moveServo(int angle, int fd);
	int getAngle();
private:
	int minValue;
	int maxValue;
	int headerNumber;
	int currAngle;
};

#endif

