compile:
	g++ rfcomm-server.cpp servoHandler.cpp -o faceTracker `pkg-config --cflags --libs opencv` -lwiringPi -lbluetooth -lpthread 

clean:
	rm ./faceTracker
