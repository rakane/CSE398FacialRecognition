#ifndef SERVOHANDER_H
#define SERVOHANDLER_H

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "wiringPiI2C.h"

class ServoHandler {
public:
	ServoHandler(int headerNum, int fd);
	int moveServo(int angle, int fd);
private:
	int MIN_VAL = 0x70;
	int MAX_VAL = 0x415;
	int headerNumber;	
};

#endif

