//run this command: sudo apt-get install libbluetooth-dev
//compile gcc -o rfcomm-server rfcomm-server.c -lbluetooth

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "bluetooth.h"
#include "rfcomm.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
#include "servoHandler.h"

//using namespace cv;
using namespace std;


//Point detectAndDisplay(Mat frame);

/** Global variables */
//string face_cascade_name = "haarcascade_frontalface_alt.xml";
//CascadeClassifier face_cascade;
//string window_name = "Capture - Face detection";
//RNG rng(12345);
//const long EVERY_FRAME = 5;


int main(int argc, char **argv)
{


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


	wiringPiSetup();
	int fd = wiringPiI2CSetup(0x40);
	ServoHandler pan = ServoHandler(1, fd, 0x1ae, 0x410);
	ServoHandler tilt = ServoHandler(0, fd, 0x19d, 0x410);
	tilt.moveServo(0, fd);
	pan.moveServo(0, fd);

	while(1) {
		// read data from the client
    	bytes_read = read(client, buf, sizeof(buf));
    	if( bytes_read > 0 ) {
        	printf("received [%s]\n", buf);
			int curr;
			switch(buf[0]) {
				case 'u':
					curr = tilt.getAngle();
					if(curr + 10 > 180) break;
					tilt.moveServo(curr + 10, fd);
					break;
				case 'd':
					curr = tilt.getAngle();
					if(curr - 10 < 0) break;
					tilt.moveServo(curr - 10, fd);
					break;
				case 'l':
					curr = pan.getAngle();
					if(curr + 10 > 180) break;
					pan.moveServo(curr + 10, fd);
					break;
				case 'r':
					curr = pan.getAngle();
					if(curr - 10 < 0) break;
					pan.moveServo(curr - 10, fd);
					break;
				default:
					printf("Invalid operation\n"); break;
			}
    	}
	}
    
    // close connection
    close(client);
    close(s);
    return 0;
}
