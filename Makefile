compile:
	g++ main.cpp servoHandler.cpp -o faceTracker `pkg-config --cflags --libs opencv` -lwiringPi 

clean:
	rm ./faceTracker
