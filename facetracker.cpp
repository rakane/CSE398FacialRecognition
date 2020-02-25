#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "servoHandler.h"

using namespace cv;
using namespace std;


Point detectAndDisplay(Mat frame);

/** Global variables */
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
const long EVERY_FRAME = 1;


int main(int argc, char* argv[]) {
	wiringPiSetup();
	int fd = wiringPiI2CSetup(0x40);
	ServoHandler pan = ServoHandler(1, fd, 0x1ae, 0x410);
	ServoHandler tilt = ServoHandler(0, fd, 0x19d, 0x410);

	int status = pan.moveServo(90, fd);
	if(status == -1) {
		std::cout << "Failed to move pan servo!\n";
	}

	status = tilt.moveServo(90, fd);
	if(status == -1) {
		std::cout << "Failed to move pan servo!\n";
	}
	
		
	VideoCapture cap(0);
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);	
	
	printf("Resolution Width: %d\n", frame_width);
	printf("Resolution Height: %d\n", frame_height);

	if(!cap.isOpened()) {
		cout << "Can not open webcam" << endl;
		exit(0);
	}

	if(!face_cascade.load(face_cascade_name)) {
		std::cout << "Cannot load face cascade\n";
		exit(0);
	}	
	

	Mat frame;
	long count = 0;

	while(1) {
		if(count % EVERY_FRAME == 0) {
			cap >> frame;
			Point face = detectAndDisplay(frame);
			int xNorm = face.x - 320;
			int yNorm = (face.y - 240) * -1;
			printf("X: %d\n", xNorm);
			printf("Y: %d\n", yNorm);
			int curr;
			
			if(xNorm != -320 && yNorm != -240) {
				if(xNorm > 0) {
					// Move Right
					curr = pan.getAngle();
					if(curr - 1 < 0) break;
					pan.moveServo(curr - 1, fd);
				} else if(xNorm < 0) {
					// Move Left
					curr = pan.getAngle();
					if(curr + 1 > 180) break;
					pan.moveServo(curr + 1, fd);
				}

				if(yNorm > 0) {
					// Move Right
					curr = tilt.getAngle();
					if(curr - 1 < 0) break;
					tilt.moveServo(curr - 1, fd);
				} else if(yNorm < 0) {
					// Move Left
					curr = tilt.getAngle();
					if(curr + 1 > 180) break;
					tilt.moveServo(curr + 1, fd);
				}

			}		

		
			int c = waitKey(10);
			if((char) c == 'c') break;

			if(count == 10000) {
				count = 0;
			}
		}
		count++;
	}

	cap.release();	
	return 1;
}

Point detectAndDisplay(Mat frame) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	
	//-- Detect faces
  	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100) );
	Point center;

  	for( size_t i = 0; i < faces.size(); i++ ) {
    	// Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		center.x = faces[i].x + faces[i].width * 0.5;
		center.y = faces[i].y + faces[i].height * 0.5;  
    	ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    	Mat faceROI = frame_gray( faces[i] );
  	}
  	//-- Show what you got
  	imshow( window_name, frame );
	return center;
}
