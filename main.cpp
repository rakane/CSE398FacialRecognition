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


void detectAndDisplay(Mat frame);

/** Global variables */
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
const long EVERY_FRAME = 5;


int main(int argc, char* argv[]) {
/*	wiringPiSetup();
	int fd = wiringPiI2CSetup(0x40);
	ServoHandler pan = ServoHandler(0, fd);
	ServoHandler tilt = ServoHandler(1, fd);

	if(argc > 2) {
		int status = pan.moveServo(atoi(argv[1]), fd);
		if(status == -1) {
			std::cout << "Failed to move pan servo!\n";
		}

		status = tilt.moveServo(atoi(argv[2]), fd);
		if(status == -1) {
			std::cout << "Failed to move pan servo!\n";
		}
	} else {
		for(int i = 0; i < 180; i++) {
			int status = pan.moveServo(i, fd);
			if(status == -1) {
				std::cout << "Failed to move pan servo!\n";
			}
			
			usleep(500);

			status = tilt.moveServo(i, fd);
			if(status == -1) {
				std::cout << "Failed to move pan servo!\n";
			}
			usleep(500000);
		}
	}	
*/
		
	VideoCapture cap(0);
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);	
	if(!cap.isOpened()) {
		cout << "Can not open webcam" << endl;
		exit(0);
	}

	Mat frame;
	if(!face_cascade.load(face_cascade_name)) {
		std::cout << "Cannot load face cascade\n";
		exit(0);
	}	
	
	long count = 0;

	while(1) {
		if(count % EVERY_FRAME == 0) {
			cap >> frame;
			detectAndDisplay(frame);
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

void detectAndDisplay(Mat frame) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	
	//-- Detect faces
  	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

  	for( size_t i = 0; i < faces.size(); i++ ) {
    	Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
    	ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    	Mat faceROI = frame_gray( faces[i] );
  	}
  	//-- Show what you got
  	imshow( window_name, frame );
}
