#include "servoHandler.h"

ServoHandler::ServoHandler(int headerNum, int fd, int minVal, int maxVal) {
	headerNumber = headerNum;
	minValue = minVal;
	maxValue = maxVal;

	int status = wiringPiI2CWriteReg8(fd, 0x00, 0x01);
	if(status == -1) {
		std::cout << "Failed to write initial setup mode!\n";
		exit(1);
	}
}

int ServoHandler::getAngle() {
	return currAngle;
}

int ServoHandler::moveServo(int angle, int fd) {
	currAngle = angle;
	int step = (maxValue - minValue) / 180;
	int mappedAngle = minValue + (step * angle);

	int lowBits = mappedAngle & 0x00FF;
	int highBits = (mappedAngle & 0xFF00) >> 8
			;	
	std::cout << "Mapped Angle: " << std::hex << mappedAngle << std::endl;
	std::cout << "Low bits: " << std::hex << lowBits << std::endl;
	std::cout << "High bits: " << std::hex << highBits << std::endl;
	
	int lowReg = 0x06 + (headerNumber * 0x04) + 2;
	int highReg = 0x06 + (headerNumber * 0x04) + 3;
	
	int status = wiringPiI2CWriteReg8(fd, lowReg, lowBits);
	if(status == -1) {
		std::cout << "Failed to write low bits!\n";
		return -1;
	}

	status = wiringPiI2CWriteReg8(fd, highReg, highBits);
	if(status == -1) {
		std::cout << "Failed to write high bits!\n";
		return -1;
	}

	return 1;
}
