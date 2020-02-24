#include "servoHandler.h"

ServoHandler::ServoHandler(int headerNum, int fd) {
	headerNumber = headerNum;

	int status = wiringPiI2CWriteReg8(fd, 0x00, 0x01);
	if(status == -1) {
		std::cout << "Failed to write initial setup mode!\n";
		exit(1);
	}
}


int ServoHandler::moveServo(int angle, int fd) {
	
	int step = (MAX_VAL - MIN_VAL) / 180;
	int mappedAngle = MIN_VAL + (step * angle);

	int lowBits = mappedAngle & 0x00FF;
	int highBits = (mappedAngle & 0xFF00) >> 8;

	std::cout << "Mapped Angle: " << std::hex << mappedAngle << std::endl;
	std::cout << "Low bits: " << std::hex << lowBits << std::endl;
	std::cout << "High bits: " << std::hex << highBits << std::endl;
	
	int lowReg = 0x06 + (headerNumber * 0x04) + 2;
	int highReg = 0x06 + (headerNumber * 0x04) + 3;
	
	int status = wiringPiI2CWriteReg8(fd, lowReg, lowBits);
	if(status == -1) {
		std::cout << "Failed to write initial setup mode!\n";
		return -1;
	}

	status = wiringPiI2CWriteReg8(fd, highReg, highBits);
	if(status == -1) {
		std::cout << "Failed to write initial setup mode!\n";
		return -1;
	}

	return 1;
}
