brightness: main.cpp
	c++ -std=c++11 -pthread main.cpp -O3 -lpigpio -lrt -lopencv_highgui -lopencv_core -o brightness
