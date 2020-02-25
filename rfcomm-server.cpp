//run this command: sudo apt-get install libbluetooth-dev
//compile gcc -o rfcomm-server rfcomm-server.c -lbluetooth

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <thread>
#include "bluetooth.h"
#include "rfcomm.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "servoHandler.h"

using namespace cv;
using namespace std;

void bluetoothSocket(ServoHandler* pan, ServoHandler* tilt); 

// Face Tracking Variables
Point detectAndDisplay(Mat frame);
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
const long EVERY_FRAME = 5;
int fd;
int trackingOn = 1;

int main(int argc, char **argv)
{

	// Initial Wiring Pi and Bluetooth Server Setup
	wiringPiSetup();
	fd = wiringPiI2CSetup(0x40);
	ServoHandler* pan = new ServoHandler(1, fd, 0x1ae, 0x410);
	ServoHandler* tilt = new ServoHandler(0, fd, 0x19d, 0x410);
	thread bluetooth_socket(bluetoothSocket, pan, tilt);
	bluetooth_socket.detach();
	int status = pan->moveServo(90, fd);
	if(status == -1) {
		std::cout << "Failed to move pan servo!\n";
	}

	status = tilt->moveServo(90, fd);
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
						curr = pan->getAngle();
						if(curr - 1 < 0) {} else {
							pan->moveServo(curr - 1, fd);
						}
					} else if(xNorm < 0) {
						// Move Left
						curr = pan->getAngle();
						if(curr + 1 > 180) {} else {
							pan->moveServo(curr + 1, fd);
						}
					}
						
					if(yNorm > 0) {
						// Move Right
						curr = tilt->getAngle();
						if(curr - 1 < 0) {} else {
							tilt->moveServo(curr - 1, fd);
						}
					} else if(yNorm <0) {
						// Move Left
						curr = tilt->getAngle();
						if(curr + 1 > 180) {} else {
							tilt->moveServo(curr + 1, fd);
						}
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
	printf("Main thread finishing...\n");
	cap.release();	
}

void bluetoothSocket(ServoHandler* pan, ServoHandler* tilt) {
 	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    //loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);
	
    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

	while(1) {
		// read data from the client
    	bytes_read = read(client, buf, sizeof(buf));
    	if( bytes_read > 0 ) {
        	printf("received [%s]\n", buf);
			int curr;
			switch(buf[0]) {
				case 'a':
					trackingOn = 1; break;
				case 'b':
					trackingOn = 0; break;
				case 'u':
					if(trackingOn == 0) {
						curr = tilt->getAngle();
						if(curr + 10 > 180) break;
						tilt->moveServo(curr + 10, fd);
					}
					break;
				case 'd':
					if(trackingOn == 0) {
						curr = tilt->getAngle();
						if(curr - 10 < 0) break;
						tilt->moveServo(curr - 10, fd);
					}
					break;
				case 'l':
					if(trackingOn == 0) {
						curr = pan->getAngle();
						if(curr + 10 > 180) break;
						pan->moveServo(curr + 10, fd);
					}
					break;
				case 'r':
					if(trackingOn == 0) {
						curr = pan->getAngle();
						if(curr - 10 < 0) break;
						pan->moveServo(curr - 10, fd);
					}
					break;
				default:
					printf("Invalid operation\n"); break;
			}
    	}
	}
    
    // close connection'
	printf("Server closing...\n");
    close(client);
    close(s);
}

Point detectAndDisplay(Mat frame) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	Point center;

	if(trackingOn == 0) {
		center.x = 0;
		center.y = 0;
		//-- Show what you got
  		imshow( window_name, frame );
		return center;
	}


	//-- Detect faces
  	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100) );

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

