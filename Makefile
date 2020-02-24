compile:
	g++ main.cpp servoHandler.cpp -o faceTracker `pkg-config --cflags --libs opencv` -lwiringPi -lbluetooth 

clean:
	rm ./faceTracker
